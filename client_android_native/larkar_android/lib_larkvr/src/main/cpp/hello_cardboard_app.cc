/*
 * Copyright 2019 Google LLC
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "hello_cardboard_app.h"

#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <android/log.h>

#include <array>
#include <cmath>
#include <fstream>
#include <lark_xr/xr_config.h>
#include <unistd.h>

#include "cardboard.h"
#include "lark_xr/pxy_inner_utils.h"
#include "lark_xr/xr_latency_collector.h"

namespace ndk_hello_cardboard {

namespace {

// The objects are about 1 meter in radius, so the min/max target distance are
// set so that the objects are always within the room (which is about 5 meters
// across) and the reticle is always closer than any objects.
constexpr float kMinTargetDistance = 2.5f;
constexpr float kMaxTargetDistance = 3.5f;
constexpr float kMinTargetHeight = 0.5f;
constexpr float kMaxTargetHeight = kMinTargetHeight + 3.0f;

constexpr float kDefaultFloorHeight = -1.7f;

constexpr uint64_t kPredictionTimeWithoutVsyncNanos = 50000000;

// Angle threshold for determining whether the controller is pointing at the
// object.
constexpr float kAngleLimit = 0.2f;

// Number of different possible targets
constexpr int kTargetMeshCount = 3;

// Simple shaders to render .obj files without any lighting.
constexpr const char* kObjVertexShader =
    R"glsl(
    uniform mat4 u_MVP;
    attribute vec4 a_Position;
    attribute vec2 a_UV;
    varying vec2 v_UV;

    void main() {
      v_UV = a_UV;
      gl_Position = u_MVP * a_Position;
    })glsl";

constexpr const char* kObjFragmentShader =
    R"glsl(
    precision mediump float;

    uniform sampler2D u_Texture;
    varying vec2 v_UV;

    void main() {
      // The y coordinate of this sample's textures is reversed compared to
      // what OpenGL expects, so we invert the y coordinate.
      gl_FragColor = texture2D(u_Texture, vec2(v_UV.x, 1.0 - v_UV.y));
    })glsl";

}  // anonymous namespace

HelloCardboardApp::HelloCardboardApp(JavaVM* vm, jobject obj,
                                     jobject asset_mgr_obj,
                                     const char* appid)
    : head_tracker_(nullptr),
      lens_distortion_(nullptr),
      distortion_renderer_(nullptr),
      screen_params_changed_(false),
      device_params_changed_(false),
      screen_width_(0),
      screen_height_(0),
      depthRenderBuffer_(0),
      framebuffer_(0),
      texture_(0),
      obj_program_(0),
      obj_position_param_(0),
      obj_uv_param_(0),
      obj_modelview_projection_param_(0),
      target_object_meshes_(kTargetMeshCount),
      target_object_not_selected_textures_(kTargetMeshCount),
      target_object_selected_textures_(kTargetMeshCount),
      cur_target_object_(RandomUniformInt(kTargetMeshCount)),
      vm_(vm),
      appid_(appid)
{
  JNIEnv* env;
  vm->GetEnv((void**)&env, JNI_VERSION_1_6);
  java_asset_mgr_ = env->NewGlobalRef(asset_mgr_obj);
  asset_mgr_ = AAssetManager_fromJava(env, asset_mgr_obj);

  Cardboard_initializeAndroid(vm, obj);
  head_tracker_ = CardboardHeadTracker_create();

  activity_ = env->NewGlobalRef(obj);
}

HelloCardboardApp::~HelloCardboardApp() {
  CardboardHeadTracker_destroy(head_tracker_);
  CardboardLensDistortion_destroy(lens_distortion_);
  CardboardDistortionRenderer_destroy(distortion_renderer_);

  if (activity_) {
    EnvWrapper env = GetEnvWrapper();
    env.get()->DeleteGlobalRef(activity_);
  }

  if (recording_stream_) {
    recording_stream_->close();
    recording_stream_.reset();
  }

  if (xr_client_) {
    xr_client_->Close();
    xr_client_->OnDestory();

    // release cloudlark
    xr_client_->UnRegisterObserver();
    xr_client_->Release();
    xr_client_.reset();
  }
#ifdef ENABLE_CLOUDXR
  if (cloudxr_client_) {
    cloudxr_client_->Teardown();
    cloudxr_client_.reset();
  }
#endif
}

void HelloCardboardApp::OnSurfaceCreated(JNIEnv* env) {
  const int obj_vertex_shader =
      LoadGLShader(GL_VERTEX_SHADER, kObjVertexShader);
  const int obj_fragment_shader =
      LoadGLShader(GL_FRAGMENT_SHADER, kObjFragmentShader);

  obj_program_ = glCreateProgram();
  glAttachShader(obj_program_, obj_vertex_shader);
  glAttachShader(obj_program_, obj_fragment_shader);
  glLinkProgram(obj_program_);
  glUseProgram(obj_program_);

  CHECKGLERROR("Obj program");

  obj_position_param_ = glGetAttribLocation(obj_program_, "a_Position");
  obj_uv_param_ = glGetAttribLocation(obj_program_, "a_UV");
  obj_modelview_projection_param_ = glGetUniformLocation(obj_program_, "u_MVP");

  CHECKGLERROR("Obj program params");

  HELLOCARDBOARD_CHECK(room_.Initialize(obj_position_param_, obj_uv_param_,
                                        "CubeRoom.obj", asset_mgr_));
  HELLOCARDBOARD_CHECK(
      room_tex_.Initialize(env, java_asset_mgr_, "CubeRoom_BakedDiffuse.png"));
  HELLOCARDBOARD_CHECK(target_object_meshes_[0].Initialize(
      obj_position_param_, obj_uv_param_, "Icosahedron.obj", asset_mgr_));
  HELLOCARDBOARD_CHECK(target_object_not_selected_textures_[0].Initialize(
      env, java_asset_mgr_, "Icosahedron_Blue_BakedDiffuse.png"));
  HELLOCARDBOARD_CHECK(target_object_selected_textures_[0].Initialize(
      env, java_asset_mgr_, "Icosahedron_Pink_BakedDiffuse.png"));
  HELLOCARDBOARD_CHECK(target_object_meshes_[1].Initialize(
      obj_position_param_, obj_uv_param_, "QuadSphere.obj", asset_mgr_));
  HELLOCARDBOARD_CHECK(target_object_not_selected_textures_[1].Initialize(
      env, java_asset_mgr_, "QuadSphere_Blue_BakedDiffuse.png"));
  HELLOCARDBOARD_CHECK(target_object_selected_textures_[1].Initialize(
      env, java_asset_mgr_, "QuadSphere_Pink_BakedDiffuse.png"));
  HELLOCARDBOARD_CHECK(target_object_meshes_[2].Initialize(
      obj_position_param_, obj_uv_param_, "TriSphere.obj", asset_mgr_));
  HELLOCARDBOARD_CHECK(target_object_not_selected_textures_[2].Initialize(
      env, java_asset_mgr_, "TriSphere_Blue_BakedDiffuse.png"));
  HELLOCARDBOARD_CHECK(target_object_selected_textures_[2].Initialize(
      env, java_asset_mgr_, "TriSphere_Pink_BakedDiffuse.png"));

  // Target object first appears directly in front of user.
  model_target_ = GetTranslationMatrix({0.0f, 1.5f, kMinTargetDistance});

  LOGI("create xr clients");
  xr_client_ = std::make_shared<lark::XRClient>();
  xr_client_->EnableDebugMode(true);

  xr_client_->Init(nullptr, true);
  xr_client_->RegisterObserver(this);
  LOGD("url %s:%hu", lark::XRClient::GetServerHost().c_str(), lark::XRClient::GetServerPort());

  xr_client_->SetServerAddr(lark::XRClient::GetServerHost(), lark::XRClient::GetServerPort());
//     xr_client_->SetServerAddr("192.168.0.55", 8181);
//     xr_client_->SetServerAddr("192.168.0.55", 8181);
//    xr_client_->SetServerAddr("192.168.0.177", 8181);

    lark::XRConfig::render_width = 1280;
    lark::XRConfig::render_height = 720;
    lark::XRConfig::fps = 60;
    lark::XRConfig::use_render_queue = false;
    lark::XRConfig::use_multiview = false;
    lark::XRConfig::room_height = 1.7;

    lark::XRConfig::QuickConfigWithDefaulSetup(lark::QuickConfigLevel_Fast);

#ifdef LARK_SDK_SECRET
  // 初始化 cloudlark sdk
    std::string timestamp = GetTimestampMillStr();
    std::string signature = lark::GetSignature(LARK_SDK_ID, LARK_SDK_SECRET, timestamp);
    if (!xr_client_->InitSdkAuthorization(LARK_SDK_ID, signature, timestamp)) {
        LOGV("init sdk auth faild %d %s", xr_client_->last_error_code(), xr_client_->last_error_message().c_str());
    }
#else
  if (!xr_client_->InitSdkAuthorization(LARK_SDK_ID)) {
    LOGV("init sdk auth faild %d %s", xr_client_->last_error_code(), xr_client_->last_error_message().c_str());
  }
#endif

#ifdef ENABLE_CLOUDXR
  cloudxr_client_ = std::make_unique<CloudXRClient>(this);
//    bool res = cloudxr_client_->Init();
//    LOGI("cloudxr init res %d", res);
#endif

// test 切水果
//  xr_client_->EnterAppli("888075472670294016");
//  xr_client_->EnterAppli("888082130460999680");

  xr_client_->EnterAppli(appid_);

  CHECKGLERROR("OnSurfaceCreated");
}

void HelloCardboardApp::SetScreenParams(int width, int height) {
  screen_width_ = width;
  screen_height_ = height;
  screen_params_changed_ = true;
}

void HelloCardboardApp::OnDrawFrame() {
  if (!UpdateDeviceParams()) {
    return;
  }

  // Update Head Pose.
  head_view_ = GetPose();

  // Incorporate the floor height into the head_view
  head_view_ =
      head_view_ * GetTranslationMatrix({0.0f, kDefaultFloorHeight, 0.0f});

#ifdef ENABLE_CLOUDXR
  if (need_reconnect_public_ip_) {
    // recreate recevier
    cloudxr_client_->Teardown();
    cloudxr_client_->Init();
    // try to connect to public ip.
    cloudxr_client_->Connect(prepare_public_ip_);
    need_reconnect_public_ip_ = false;
    prepare_public_ip_ = "";
  }
  // cloudxr prepare render
  bool have_frame = false;

  if (cloudxr_client_->IsConnect()) {
    const cxrError status = cloudxr_client_->Latch();
    if (status != cxrError_Success) {
      LOGE("Latch failed, %s", cxrErrorString(status));
      if (status == cxrError_Receiver_Not_Running) {
        return;
      }
      else if (status == cxrError_Frame_Not_Ready) {
        // TODO: if fixed framerate, should cache and re-render prior frame.
        //  if not fixed, can just not swap backbuffer.
      }
      // else
      // TODO: code should handle other potential errors that are non-fatal, but
      //  may be enough to need to disconnect or reset view or other interruption cases.
    }
    have_frame = (status == cxrError_Success);

    // TODO setup offset
    const int pose_offset = have_frame ? cloudxr_client_->DetermineOffset() : 0;
  }
#endif

  // Bind buffer
  glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_);

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glDisable(GL_SCISSOR_TEST);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Draw eyes views
  for (int eye = 0; eye < 2; ++eye) {
    glViewport(eye == kLeft ? 0 : screen_width_ / 2, 0, screen_width_ / 2,
               screen_height_);

    Matrix4x4 eye_matrix = GetMatrixFromGlArray(eye_matrices_[eye]);
    Matrix4x4 eye_view = eye_matrix * head_view_;

    Matrix4x4 projection_matrix =
        GetMatrixFromGlArray(projection_matrices_[eye]);
    Matrix4x4 modelview_target = eye_view * model_target_;
    modelview_projection_target_ = projection_matrix * modelview_target;
    modelview_projection_room_ = projection_matrix * eye_view;

    // Draw room and target
    // TODO ADD LOCALRENDER LOADING
    DrawWorld();

#ifdef ENABLE_CLOUDXR
    if (have_frame) {
      cloudxr_client_->Render(eye == 0 ? CloudXRClient::FrameMask_Left : CloudXRClient::FrameMask_Right);
    }
#endif

    // draw cloud when cloud texture ready
    if (xr_client_->media_ready()) {
      // latency
      lark::XRLatencyCollector::Instance().Rendered2(trackingFrame_.frameIndex);

      // draw
      xr_client_->Draw(static_cast<larkxrEye>(eye));

      // latency
      lark::XRLatencyCollector::Instance().Submit(trackingFrame_.frameIndex, 0);
    }
  }

  // Render
  CardboardDistortionRenderer_renderEyeToDisplay(
      distortion_renderer_, /* target_display = */ 0, /* x = */ 0, /* y = */ 0,
      screen_width_, screen_height_, &left_eye_texture_description_,
      &right_eye_texture_description_);

#ifdef ENABLE_CLOUDXR
  if (have_frame) {
    cloudxr_client_->Release();
    cloudxr_client_->Stats();
  }
#endif

  CHECKGLERROR("onDrawFrame");
}

void HelloCardboardApp::OnTriggerEvent() {
  if (IsPointingAtTarget()) {
    HideTarget();
  }
}

void HelloCardboardApp::OnPause() { CardboardHeadTracker_pause(head_tracker_); }

void HelloCardboardApp::OnResume() {
  CardboardHeadTracker_resume(head_tracker_);

  // Parameters may have changed.
  device_params_changed_ = true;

  // Check for device parameters existence in external storage. If they're
  // missing, we must scan a Cardboard QR code and save the obtained parameters.
  uint8_t* buffer;
  int size;
  CardboardQrCode_getSavedDeviceParams(&buffer, &size);
  if (size == 0) {
    // TODO switch view when setup. not 3d activity.
    SwitchViewer();
  }
  CardboardQrCode_destroy(buffer);
}

void HelloCardboardApp::SwitchViewer() {
  CardboardQrCode_scanQrCodeAndSaveDeviceParams();
}

bool HelloCardboardApp::UpdateDeviceParams() {
  // Checks if screen or device parameters changed
  if (!screen_params_changed_ && !device_params_changed_) {
    return true;
  }

  // Get saved device parameters
  uint8_t* buffer;
  int size;
  CardboardQrCode_getSavedDeviceParams(&buffer, &size);

  // If there are no parameters saved yet, returns false.
  if (size == 0) {
    return false;
  }

  CardboardLensDistortion_destroy(lens_distortion_);
  lens_distortion_ = CardboardLensDistortion_create(buffer, size, screen_width_,
                                                    screen_height_);

  CardboardQrCode_destroy(buffer);

  GlSetup();

  CardboardDistortionRenderer_destroy(distortion_renderer_);
  distortion_renderer_ = CardboardOpenGlEs2DistortionRenderer_create();

  CardboardMesh left_mesh;
  CardboardMesh right_mesh;
  CardboardLensDistortion_getDistortionMesh(lens_distortion_, kLeft,
                                            &left_mesh);
  CardboardLensDistortion_getDistortionMesh(lens_distortion_, kRight,
                                            &right_mesh);

  CardboardDistortionRenderer_setMesh(distortion_renderer_, &left_mesh, kLeft);
  CardboardDistortionRenderer_setMesh(distortion_renderer_, &right_mesh,
                                      kRight);

  // Get eye matrices
  CardboardLensDistortion_getEyeFromHeadMatrix(lens_distortion_, kLeft,
                                               eye_matrices_[0]);
  CardboardLensDistortion_getEyeFromHeadMatrix(lens_distortion_, kRight,
                                               eye_matrices_[1]);
  CardboardLensDistortion_getProjectionMatrix(lens_distortion_, kLeft, kZNear,
                                              kZFar, projection_matrices_[0]);
  CardboardLensDistortion_getProjectionMatrix(lens_distortion_, kRight, kZNear,
                                              kZFar, projection_matrices_[1]);

  screen_params_changed_ = false;
  device_params_changed_ = false;

  // setup cloud params
  LOGI("setup cloud params");
  lark::XRConfig::render_width = 1280;
  lark::XRConfig::render_height = 720;
  lark::XRConfig::fps = 60;

  const double rr = 180 / M_PI;
  for (int eye = 0; eye < 2; ++eye) {
    Matrix4x4 projection_matrix = GetMatrixFromGlArray(projection_matrices_[eye]);
    glm::mat4 proj = toGlm(projection_matrix);

    float left_tan, right_tan, top_tan, bottom_tan;
    if (fabsf(proj[2][0]) > 0.0001f) {
      // Non-symmetric projection
      const float oneOver00 = 1.f/proj[0][0];
      const float l = -(1.f - proj[2][0])*oneOver00;
      const float r = 2.f*oneOver00 + l;

      const float oneOver11 = 1.f/proj[1][1];
      const float b = -(1.f - proj[2][1])*oneOver11;
      const float t = 2.f*oneOver11 + b;

      LOGV("Non-symmetric projection [%d] fov l %f r %f t %f b %f", eye, l, r, t, b);

      left_tan = l;
      right_tan = r;
      top_tan = -t;
      bottom_tan = -b;
    } else {
      // Symmetric projection
      left_tan = -1.f/proj[0][0];
      right_tan = -left_tan;
      top_tan = -1.f/proj[1][1];
      bottom_tan = -top_tan;

      LOGV("Symmetric eye[%d] fov l %f r %f t %f b %f", eye, left_tan, right_tan, top_tan, bottom_tan);
    }

    float left = abs(atan(left_tan) * rr); // left (minX)
    float right = abs(atan(right_tan) * rr); // right (maxX)
    float top = abs(atan(top_tan) * rr); // top (minY)
    float bottom = abs(atan(bottom_tan) * rr); // bottom (maxY)

    lark::XRConfig::fov[eye].left = left;
    lark::XRConfig::fov[eye].right = right;
    lark::XRConfig::fov[eye].top = top;
    lark::XRConfig::fov[eye].bottom = bottom;

    LOGV("setup eye[%d] fov l %f r %f t %f b %f", eye, left, right, top, bottom);
#ifdef ENABLE_CLOUDXR
    cloudxr_client_->SetProjectionMatrix(eye, proj);
#endif
  }

#ifdef ENABLE_CLOUDXR
  cloudxr_client_->SetStreamRes(screen_width_, screen_height_, 0);
//
  bool res = cloudxr_client_->Init();
  LOGI("cloudxr init res %d", res);

//    cloudxr_client_->Connect("192.168.0.50");
#endif


  CHECKGLERROR("UpdateDeviceParams");

  return true;
}

void HelloCardboardApp::GlSetup() {
  LOGD("GL SETUP");

  if (framebuffer_ != 0) {
    GlTeardown();
  }

  // Create render texture.
  glGenTextures(1, &texture_);
  glBindTexture(GL_TEXTURE_2D, texture_);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, screen_width_, screen_height_, 0,
               GL_RGB, GL_UNSIGNED_BYTE, 0);

  left_eye_texture_description_.texture = texture_;
  left_eye_texture_description_.left_u = 0;
  left_eye_texture_description_.right_u = 0.5;
  left_eye_texture_description_.top_v = 1;
  left_eye_texture_description_.bottom_v = 0;

  right_eye_texture_description_.texture = texture_;
  right_eye_texture_description_.left_u = 0.5;
  right_eye_texture_description_.right_u = 1;
  right_eye_texture_description_.top_v = 1;
  right_eye_texture_description_.bottom_v = 0;

  // Generate depth buffer to perform depth test.
  glGenRenderbuffers(1, &depthRenderBuffer_);
  glBindRenderbuffer(GL_RENDERBUFFER, depthRenderBuffer_);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, screen_width_,
                        screen_height_);
  CHECKGLERROR("Create Render buffer");

  // Create render target.
  glGenFramebuffers(1, &framebuffer_);
  glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                         texture_, 0);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                            GL_RENDERBUFFER, depthRenderBuffer_);

  CHECKGLERROR("GlSetup");
}

void HelloCardboardApp::GlTeardown() {
  if (framebuffer_ == 0) {
    return;
  }
  glDeleteRenderbuffers(1, &depthRenderBuffer_);
  depthRenderBuffer_ = 0;
  glDeleteFramebuffers(1, &framebuffer_);
  framebuffer_ = 0;
  glDeleteTextures(1, &texture_);
  texture_ = 0;

  CHECKGLERROR("GlTeardown");
}

Matrix4x4 HelloCardboardApp::GetPose() {
  std::array<float, 4> out_orientation;
  std::array<float, 3> out_position;
  CardboardHeadTracker_getPose(
      head_tracker_, GetBootTimeNano() + kPredictionTimeWithoutVsyncNanos,
      kLandscapeLeft, &out_position[0], &out_orientation[0]);
  return GetTranslationMatrix(out_position) *
         Quatf::FromXYZW(&out_orientation[0]).ToMatrix();
}

void HelloCardboardApp::DrawWorld() {
  DrawRoom();
  DrawTarget();
}

void HelloCardboardApp::DrawTarget() {
  glUseProgram(obj_program_);

  std::array<float, 16> target_array = modelview_projection_target_.ToGlArray();
  glUniformMatrix4fv(obj_modelview_projection_param_, 1, GL_FALSE,
                     target_array.data());

  if (IsPointingAtTarget()) {
    target_object_selected_textures_[cur_target_object_].Bind();
  } else {
    target_object_not_selected_textures_[cur_target_object_].Bind();
  }
  target_object_meshes_[cur_target_object_].Draw();

  CHECKGLERROR("DrawTarget");
}

void HelloCardboardApp::DrawRoom() {
  glUseProgram(obj_program_);

  std::array<float, 16> room_array = modelview_projection_room_.ToGlArray();
  glUniformMatrix4fv(obj_modelview_projection_param_, 1, GL_FALSE,
                     room_array.data());

  room_tex_.Bind();
  room_.Draw();

  CHECKGLERROR("DrawRoom");
}

void HelloCardboardApp::HideTarget() {
  cur_target_object_ = RandomUniformInt(kTargetMeshCount);

  float angle = RandomUniformFloat(-M_PI, M_PI);
  float distance = RandomUniformFloat(kMinTargetDistance, kMaxTargetDistance);
  float height = RandomUniformFloat(kMinTargetHeight, kMaxTargetHeight);
  std::array<float, 3> target_position = {std::cos(angle) * distance, height,
                                          std::sin(angle) * distance};

  model_target_ = GetTranslationMatrix(target_position);
}

bool HelloCardboardApp::IsPointingAtTarget() {
  // Compute vectors pointing towards the reticle and towards the target object
  // in head space.
  Matrix4x4 head_from_target = head_view_ * model_target_;

  const std::array<float, 4> unit_quaternion = {0.f, 0.f, 0.f, 1.f};
  const std::array<float, 4> point_vector = {0.f, 0.f, -1.f, 0.f};
  const std::array<float, 4> target_vector = head_from_target * unit_quaternion;

  float angle = AngleBetweenVectors(point_vector, target_vector);
  return angle < kAngleLimit;
}

//
// callbacks
//
void HelloCardboardApp::OnCloudXRReady(const std::string &appServerIp,
                                       const std::string &perferOutIp) {
    XRClientObserverWrap::OnCloudXRReady(appServerIp, perferOutIp);
      LOGI("OnCloudXRReady appServerIp %s perferOutIp %s", appServerIp.c_str(), perferOutIp.c_str());
#ifdef ENABLE_CLOUDXR
      prepare_public_ip_ = perferOutIp;
      cloudxr_client_->Connect(appServerIp);
//    cloudxr_client_->Connect("192.168.0.50");
#else
      LOGE("MUST ENABLE CLOUDXR AND PUT CloudXR SDK under projectroot/libs folder.");
#endif
}

void HelloCardboardApp::RequestTrackingInfo() {
  XRClientObserverWrap::RequestTrackingInfo();

  glm::mat4 head = toGlm(head_view_);
  glm::quat rotate = glm::quat_cast(head);
  glm::vec3 position;
  position.x = head[3][0];
  position.y = head[3][1];
  position.z = head[3][2];

  static uint64_t frameIndex = 0;
  frameIndex++;
  larkxrTrackingDevicePairFrame tracking = {};
  tracking.frameIndex = frameIndex;
  tracking.devicePair.hmdPose.isConnected = true;
  /* invert view matrix */
  tracking.devicePair.hmdPose.rotation = rotate;
  tracking.devicePair.hmdPose.position = position;
  tracking.devicePair.hmdPose.rawPoseMatrix = head;
  // send test pose;
  xr_client_->SendDevicePair(tracking);
}

void HelloCardboardApp::OnConnected() {
  XRClientObserverWrap::OnConnected();
  LOGI("on xr client OnConnected");
  JavaShowToast("连接成功，正在启动云端应用");
}

void HelloCardboardApp::OnClose(int code) {
  XRClientObserverWrap::OnClose(code);
  if (recording_stream_) {
    recording_stream_->close();
    recording_stream_.reset();
  }
}

void HelloCardboardApp::OnTrackingFrame(const larkxrTrackingFrame &trackingFrame) {
  XRClientObserverWrap::OnTrackingFrame(trackingFrame);
  trackingFrame_ = trackingFrame;
}

void HelloCardboardApp::OnInfo(int infoCode, const std::string &msg) {
  XRClientObserverWrap::OnInfo(infoCode, msg);
  LOGI("on xr client info %d; msg %s;", infoCode, msg.c_str());
  JavaShowToast(msg.c_str());
}

void HelloCardboardApp::OnError(int errCode, const std::string &msg) {
  XRClientObserverWrap::OnError(errCode, msg);
  LOGE("on xr client error %d; msg %s;", errCode, msg.c_str());
  JavaOnError(msg.c_str());
}

void HelloCardboardApp::OnDataChannelOpen() {
  XRClientObserverWrap::OnDataChannelOpen();
  LOGI("OnDataChannelOpen");
}

#ifdef ENABLE_CLOUDXR
void HelloCardboardApp::UpdateClientState(cxrClientState state, cxrStateReason reason) {
  LOGI("UpdateClientState state %d reason %d", state, reason);
  switch (state) {
    case cxrClientState_ReadyToConnect:
      JavaShowToast("创建CloudXR客户端成功");
      break;
    case cxrClientState_ConnectionAttemptInProgress:
      JavaShowToast("开始连接服务器");
      break;
    case cxrClientState_StreamingSessionInProgress:
      JavaShowToast("连接服务器成功");
      break;
    case cxrClientState_ConnectionAttemptFailed:
      {
        if (!prepare_public_ip_.empty()) {
          need_reconnect_public_ip_ = true;
        } else {
          char buff[200];
          sprintf(buff, "连接CloudXR服务器失败 reason %d", reason);
          JavaOnError(buff);
        }
      }
      break;
    case cxrClientState_Disconnected:
      {
        char buff[200];
        sprintf(buff, "与CloudXR服务器连接断开 reason %d", reason);
        JavaOnError(buff);
      }
      break;
    default:
      break;
  }
}

void HelloCardboardApp::ReceiveUserData(const void *data, uint32_t size) {
  // ReceiveUserData here
  LOGI("cloudxr ReceiveUserData ");
}

void HelloCardboardApp::GetTrackingState(glm::mat4 *post_matrix) {
  *post_matrix = toGlm(head_view_);
}
#endif

oboe::DataCallbackResult
HelloCardboardApp::onAudioReady(oboe::AudioStream *oboeStream, void *audioData,
                                int32_t numFrames) {
  //    LOGV("onAudioReady %d", numFrames);
  if (!xr_client_ || !xr_client_->is_connected()) {
    LOGV("skip on audio ready %d", numFrames);
    return oboe::DataCallbackResult::Continue;
  }
  int streamSizeBytes = numFrames * AUDIO_CHANNEL_COUNT * AUDIO_SAMPLE_SIZE;

//    LOGV("onAudioReady %d", streamSizeBytes);

  xr_client_->SendAudioData(static_cast<const char *>(audioData), streamSizeBytes);

  return oboe::DataCallbackResult::Continue;
}

void HelloCardboardApp::RequestAudioInput() {
  XRClientObserverWrap::RequestAudioInput();

  LOGV("RequestAudioInput");

  // Initialize audio recording
  oboe::AudioStreamBuilder recording_stream_builder;
  recording_stream_builder.setDirection(oboe::Direction::Input);
  recording_stream_builder.setPerformanceMode(oboe::PerformanceMode::LowLatency);
  recording_stream_builder.setSharingMode(oboe::SharingMode::Exclusive);
  recording_stream_builder.setFormat(oboe::AudioFormat::I16);
  recording_stream_builder.setChannelCount(oboe::ChannelCount::Stereo);
  recording_stream_builder.setSampleRate(AUDIO_SAMPLING_RATE);
  recording_stream_builder.setInputPreset(oboe::InputPreset::VoiceCommunication);
  recording_stream_builder.setDataCallback(this);

  oboe::Result r = recording_stream_builder.openStream(recording_stream_);
  if (r != oboe::Result::OK) {
    LOGE("Failed to open recording stream. Error: %s", oboe::convertToText(r));
    //return; // for now continue to run...
  }
  else
  {
    r = recording_stream_->start();
    if (r != oboe::Result::OK)
    {
      LOGE("Failed to start recording stream. Error: %s", oboe::convertToText(r));
      //return; // for now continue to run...
    } else {
      LOGV("Start recod stream success");
    }
  }

  // if there was an error setting up, turn off sending audio for this connection.
  if (r != oboe::Result::OK) {
    LOGV("clear recod stream when failed");
    if (recording_stream_) {
      recording_stream_->close();
      recording_stream_.reset();
    }
  } else {
    LOGV("start recod stream success");
  }
}
}  // namespace ndk_hello_cardboard
