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

#ifndef HELLO_CARDBOARD_ANDROID_SRC_MAIN_JNI_HELLO_CARDBOARD_APP_H_
#define HELLO_CARDBOARD_ANDROID_SRC_MAIN_JNI_HELLO_CARDBOARD_APP_H_

#include <android/asset_manager.h>
#include <jni.h>

#include <memory>
#include <string>
#include <thread>
#include <vector>

#include <GLES2/gl2.h>
#include "cardboard.h"
#include "util.h"

#include "lark_xr/xr_client.h"
#include "oboe/Oboe.h"

#ifdef ENABLE_CLOUDXR
#include "cloudxr_client.h"
#endif

//#define LARK_SDK_ID "如果没有 SDK 授权码，联系 business@pingxingyun.com 获取,注意是 SDK 本身的授权码，不是服务器上的授权"
#ifndef LARK_SDK_ID
// 请将 SDK ID 填入第 39 行 LARK_SDK_ID 中，并放开第 39 行注释
#error "请将 SDK ID 填入第 39 行 LARK_SDK_ID 中，并放开第 39 行注释;如果没有 SDK 授权码，联系 business@pingxingyun.com 获取,注意是 SDK 本身的授权码，不是服务器上的授权"
#endif

namespace ndk_hello_cardboard {

/**
 * This is a sample app for the Cardboard SDK. It loads a simple environment and
 * objects that you can click on.
 */
class HelloCardboardApp :
        public lark::XRClientObserverWrap
        , public oboe::AudioStreamDataCallback
#ifdef ENABLE_CLOUDXR
        ,public CloudXRClientObserver
#endif
{
 public:
  /**
   * Creates a HelloCardboardApp.
   *
   * @param vm JavaVM pointer.
   * @param obj Android activity object.
   * @param asset_mgr_obj The asset manager object.
   */
  HelloCardboardApp(JavaVM* vm, jobject obj, jobject asset_mgr_obj, const char* appid);

  ~HelloCardboardApp();

  /**
   * Initializes any GL-related objects. This should be called on the rendering
   * thread with a valid GL context.
   *
   * @param env The JNI environment.
   */
  void OnSurfaceCreated(JNIEnv* env);

  /**
   * Sets screen parameters.
   *
   * @param width Screen width
   * @param height Screen height
   */
  void SetScreenParams(int width, int height);

  /**
   * Draws the scene. This should be called on the rendering thread.
   */
  void OnDrawFrame();

  /**
   * Hides the target object if it's being targeted.
   */
  void OnTriggerEvent();

  /**
   * Pauses head tracking.
   */
  void OnPause();

  /**
   * Resumes head tracking.
   */
  void OnResume();

  /**
   * Allows user to switch viewer.
   */
  void SwitchViewer();

 private:
  /**
   * Default near clip plane z-axis coordinate.
   */
  static constexpr float kZNear = 0.1f;

  /**
   * Default far clip plane z-axis coordinate.
   */
  static constexpr float kZFar = 100.f;

  /**
   * Updates device parameters, if necessary.
   *
   * @return true if device parameters were successfully updated.
   */
  bool UpdateDeviceParams();

  /**
   * Initializes GL environment.
   */
  void GlSetup();

  /**
   * Deletes GL environment.
   */
  void GlTeardown();

  /**
   * Gets head's pose as a 4x4 matrix.
   *
   * @return matrix containing head's pose.
   */
  Matrix4x4 GetPose();

  /**
   * Draws all world-space objects for the given eye.
   */
  void DrawWorld();

  /**
   * Draws the target object.
   */
  void DrawTarget();

  /**
   * Draws the room.
   */
  void DrawRoom();

  /**
   * Finds a new random position for the target object.
   */
  void HideTarget();

  /**
   * Checks if user is pointing or looking at the target object by calculating
   * whether the angle between the user's gaze and the vector pointing towards
   * the object is lower than some threshold.
   *
   * @return true if the user is pointing at the target object.
   */
  bool IsPointingAtTarget();


    // hw decoder callback textrue.
    virtual void OnCloudXRReady(const std::string& appServerIp, const std::string& perferOutIp) override;
    virtual void OnConnected() override;
    virtual void OnClose(int code) override;
    virtual void RequestTrackingInfo() override;
    virtual void OnTrackingFrame(const larkxrTrackingFrame& trackingFrame) override;
    virtual void OnInfo(int infoCode, const std::string& msg) override;
    virtual void OnError(int errCode, const std::string& msg) override;
    virtual void OnDataChannelOpen() override;
    // update server 3.2.5.0
    virtual void RequestAudioInput() override;

    // AudioStreamDataCallback interface
    oboe::DataCallbackResult onAudioReady(oboe::AudioStream *oboeStream,
                                          void *audioData, int32_t numFrames) override;

#ifdef ENABLE_CLOUDXR
        // cloudxr callback
        virtual void UpdateClientState(cxrClientState state, cxrStateReason reason) override;
        virtual void ReceiveUserData(const void* data, uint32_t size) override;
        virtual void GetTrackingState(glm::mat4* post_matrix) override;
#endif
private:
  inline EnvWrapper GetEnvWrapper() {
    JNIEnv *env = nullptr;
    bool detached = vm_->GetEnv((void **) &env, JNI_VERSION_1_6) == JNI_EDETACHED;
    return EnvWrapper(vm_, env, detached);
  }
  inline void JavaShowToast(const char* message) {
    if (activity_ == nullptr) {
      return;
    }
    EnvWrapper env = GetEnvWrapper();
    jclass clazz = env.get()->GetObjectClass(activity_);
    jmethodID mid = env.get()->GetMethodID(clazz, "showToast", "(Ljava/lang/String;)V");

    jstring jstr = env.get()->NewStringUTF(message);
    env.get()->CallVoidMethod(activity_, mid, jstr);

    env.get()->DeleteLocalRef(jstr);
    env.get()->DeleteLocalRef(clazz);
  }
  inline void JavaOnError(const char* error) {
    if (activity_ == nullptr) {
      return;
    }
    EnvWrapper env = GetEnvWrapper();
    jclass clazz = env.get()->GetObjectClass(activity_);
    jmethodID mid = env.get()->GetMethodID(clazz, "onError", "(Ljava/lang/String;)V");

    jstring jstr = env.get()->NewStringUTF(error);
    env.get()->CallVoidMethod(activity_, mid, jstr);

    env.get()->DeleteLocalRef(jstr);
    env.get()->DeleteLocalRef(clazz);
  }

  jobject java_asset_mgr_;
  AAssetManager* asset_mgr_;

  CardboardHeadTracker* head_tracker_;
  CardboardLensDistortion* lens_distortion_;
  CardboardDistortionRenderer* distortion_renderer_;

  CardboardEyeTextureDescription left_eye_texture_description_;
  CardboardEyeTextureDescription right_eye_texture_description_;

  bool screen_params_changed_;
  bool device_params_changed_;
  int screen_width_;
  int screen_height_;

  float projection_matrices_[2][16];
  float eye_matrices_[2][16];

  GLuint depthRenderBuffer_;  // depth buffer
  GLuint framebuffer_;        // framebuffer object
  GLuint texture_;            // distortion texture

  GLuint obj_program_;
  GLuint obj_position_param_;
  GLuint obj_uv_param_;
  GLuint obj_modelview_projection_param_;

  Matrix4x4 head_view_;
  Matrix4x4 model_target_;

  Matrix4x4 modelview_projection_target_;
  Matrix4x4 modelview_projection_room_;

  TexturedMesh room_;
  Texture room_tex_;

  std::vector<TexturedMesh> target_object_meshes_;
  std::vector<Texture> target_object_not_selected_textures_;
  std::vector<Texture> target_object_selected_textures_;
  int cur_target_object_;

  // java activity callback
  JavaVM* vm_;
  jobject activity_;

  // lark lient
  std::string appid_;
  std::shared_ptr<lark::XRClient> xr_client_ = nullptr;
  larkxrTrackingFrame trackingFrame_;

  std::shared_ptr<oboe::AudioStream> recording_stream_{};
#ifdef ENABLE_CLOUDXR
        std::unique_ptr<CloudXRClient> cloudxr_client_ = nullptr;
        std::string prepare_public_ip_ = "";
        bool need_reconnect_public_ip_ = false;
#endif
};

}  // namespace ndk_hello_cardboard

#endif  // HELLO_CARDBOARD_ANDROID_SRC_MAIN_JNI_HELLO_CARDBOARD_APP_H_
