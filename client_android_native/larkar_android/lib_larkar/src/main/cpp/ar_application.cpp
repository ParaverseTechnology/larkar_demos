//
// Created by Hayasi-Yumito on 2021/12/14.
//


#include <util.h>
#include <EGL/egl.h>
#include <android/native_window_jni.h>
#include "ar_application.h"
#include <util.h>
#include <lark_xr/xr_latency_collector.h>
#include "lark_xr/xr_config.h"
#include "jni_interface.h"
#include "arcore/arcore_manager.h"
#include "hw_arengine/arengine_manager.h"

#define LOG_TAG "ArApplication"

namespace {
    const int AR_SDK_TYPE_ARCORE = 1;
    const int AR_SDK_TYPE_HW_ARENGINE = 2;

    const int JSON_COMMAND_TOUCH_DOWN = 1000;
    const int JSON_COMMAND_TOUCH_MOVE = 1001;
    const int JSON_COMMAND_TOUCH_UP   = 1002;
}

ArApplication::ArApplication(const std::string& appid, int sdkType):
    appid_(appid)
{
    LOGD("ArApplication::ArApplication() appid=%s", appid_.c_str());
    if (sdkType == AR_SDK_TYPE_HW_ARENGINE) {
        LOGD("ArApplication::ArApplication() use ArEngineManager");
        ar_manager_ = std::make_unique<ArEngineManager>();
    } else {
        LOGD("ArApplication::ArApplication() use ArcoreManager");
        ar_manager_ = std::make_unique<ArcoreManager>();
    }
}

ArApplication::~ArApplication() {
    LOGD("lark_DestroyDemo");

#ifdef ENABLE_CLOUDXR
    if (cloudxr_client_) {
        cloudxr_client_->Teardown();
        cloudxr_client_.reset();
    }
#endif

    if (xr_client_) {
        xr_client_->Close();
        xr_client_->OnDestory();

        // release cloudlark
        xr_client_->UnRegisterObserver();
        xr_client_->Release();
        xr_client_.reset();
    }

    ar_manager_->OnDestory();
    ar_manager_ = nullptr;
};

void ArApplication::OnResume(void *env, void *context, void *activity) {
    LOGI("WorldArApplication::OnResume()");
    ar_manager_->OnResume(static_cast<JNIEnv *>(env), context, activity);
}

void ArApplication::OnPause() {
    if (xr_client_) {
        xr_client_->OnPause();
    }
    ar_manager_->OnPause();
}

void ArApplication::OnSurfaceCreated() {
    LOGI("ArApplication::OnSurfaceCreated()");
    ar_manager_->OnSurfaceCreated();

    LOGD("ArApplication::InitXr()");
    xr_client_ = std::make_shared<lark::XRClient>();
    xr_client_->Init(nullptr, true);
    xr_client_->RegisterObserver(this);
    xr_client_->EnableDebugMode(true);
    LOGD("url %s:%hu", lark::XRClient::GetServerHost().c_str(), lark::XRClient::GetServerPort());

     xr_client_->SetServerAddr(lark::XRClient::GetServerHost(), lark::XRClient::GetServerPort());
//     xr_client_->SetServerAddr("192.168.0.161", 8181);
//     xr_client_->SetServerAddr("192.168.0.55", 8181);
//    xr_client_->SetServerAddr("192.168.0.177", 8181);

    if (!xr_client_->InitSdkAuthorization(LARK_SDK_ID)) {
        LOGE("init sdk auth faild %d %s", xr_client_->last_error_code(), xr_client_->last_error_message().c_str());
    }

#ifdef ENABLE_CLOUDXR
    cloudxr_client_ = std::make_unique<CloudXRClient>(this);
//    bool res = cloudxr_client_->Init();
//    LOGI("cloudxr init res %d", res);
#endif
}

void ArApplication::OnDisplayGeometryChanged(int displayRotation, int width, int height) {
    LOGI("OnDisplayGeometryChanged(%d, %d) %d", width, height, displayRotation);
    glViewport(0, 0, width, height);
    if (width > height) {
        width_ = width;
        height_ = height;
    } else {
        width_ = height;
        height_ = width;
    }

    ar_manager_->OnDisplayGeometryChanged(displayRotation, width, height);

    lark::XRConfig::fps = 60;
    lark::XRConfig::render_width = width_;
    lark::XRConfig::render_height = height_;

    lark::XRConfig::QuickConfigWithDefaulSetup(lark::QuickConfigLevel_Fast);

    // setup fov
    glm::mat4 projection = ar_manager_->project_mat();
    lark::XRConfig::SetupFovWithProjectMatrix(LARKXR_EYE_LEFT, glm::value_ptr(projection));
    lark::XRConfig::SetupFovWithProjectMatrix(LARKXR_EYE_RIGHT, glm::value_ptr(projection));
    fov_w_ = lark::XRConfig::fov[0].left + lark::XRConfig::fov[0].right;
    fov_h_ = lark::XRConfig::fov[0].top + lark::XRConfig::fov[0].bottom;
    LOGI("default getFov w=%f h=%f", fov_w_, fov_h_);


#ifdef ENABLE_CLOUDXR
    cloudxr_client_->SetStreamRes(width, height, displayRotation);

    bool support_60_fps = ar_manager_->Set60FPS();

    cloudxr_client_->SetFps(support_60_fps ? 60 : 30);
    cloudxr_client_->SetProjectionMatrix(projection);
//
    bool res = cloudxr_client_->Init();
    LOGI("cloudxr init res %d", res);

//    cloudxr_client_->Connect("192.168.0.50");
#endif
}


void ArApplication::OnDrawFrame() {
//    LOGI("ArApplication::OnDrawFrame()");
    // latency
    lark::XRLatencyCollector::Instance().Rendered2(trackingFrame_.frameIndex);

#ifdef ENABLE_CLOUDXR
    if (need_reconnect_public_ip_) {
        // recreate recevier
        cloudxr_client_->Teardown();
        cloudxr_client_->Init();
//        // try to connect to public ip.
        cloudxr_client_->Connect(prepare_public_ip_);
//
        need_reconnect_public_ip_ = false;
        prepare_public_ip_ = "";
    }
    // cloudxr prepare render
    bool have_frame = false;
    // We need to (re)calibrate but CloudXR client is running - continue
    // pulling the frames. There'll be a lag otherwise.
    if (!ar_manager_->HasChoosenAnchor() && cloudxr_client_->IsConnect()) {
        if (cloudxr_client_->Latch()==cxrError_Success)
            cloudxr_client_->Release();
    }
    if (ar_manager_->HasChoosenAnchor() && cloudxr_client_->IsConnect()) {
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

    bool res = ar_manager_->OnDrawFrame(!ar_manager_->HasChoosenAnchor(), false);

#ifdef ENABLE_CLOUDXR
    // cloudxr render
    if (res && ar_manager_->HasChoosenAnchor() && cloudxr_client_->IsConnect()) {
        // TODO update light
        // cloudxr_client_->UpdateLightProps(direction, intensity, ambient_spherical_harmonics);

        // Set light intensity to default. Intensity value ranges from 0.0f to 1.0f.
        // The first three components are color scaling factors.
        // The last one is the average pixel intensity in gamma space.
        float color_correction[4] = {1.f, 1.f, 1.f, 0.466f};
        if (have_frame) {
            // Composite CloudXR frame to the screen
            glViewport(0, 0, width_, height_);
            cloudxr_client_->Render(color_correction);
            cloudxr_client_->Release();
            cloudxr_client_->Stats();
        }
    }
#endif

    if (res && ar_manager_->HasChoosenAnchor() && xr_client_->media_ready()) {
        xr_client_->Draw();
    }

    lark::XRLatencyCollector::Instance().Submit(trackingFrame_.frameIndex, 0);
}

void ArApplication::OnTouched(float x, float y, jboolean longtap) {
    LOGD("OnTouched x:%f y:%f long:%d; choosen %d", x, y, longtap, ar_manager_->HasChoosenAnchor() );
#ifdef ENABLE_CLOUDXR
    if (ar_manager_->HasChoosenAnchor() && cloudxr_client_->IsConnect() && !longtap) {
        cloudxr_client_->HandleTouch(x, y);
    }
#endif

    if (longtap) {
        ar_manager_->ReleaseChoosenAnchor();
    } else {
        if (!ar_manager_->HasChoosenAnchor() && ar_manager_->HitTest(x, y)) {
            if (!xr_client_->is_connected()) {
#if 1
                LOGD("EnterAppli %s", appid_.c_str());
                xr_client_->EnterAppli(appid_);
#else
                        // WARNING only for debug
                        // connnect to test server.
                        CommonConfig config = {};
                        config.bitrateKbps = 8 * 1000;
                        config.fps = 60;
                        config.noOperationTimeout = 0;

                        config.ipd = 0.064F;
                        config.hasTouchcontroller = false;
                        config.fovList[0] = { 45.0F, 45.0F, 45.0F, 45.0F };
                        config.fovList[1] = { 45.0F, 45.0F, 45.0F, 45.0F };

                        config.useKcp = true;
                        config.useH265 = false;

                        config.playerMode = PlayerModeType_Normal;
                        config.userType = UserType_Player;
                        config.nickName = "";
                        config.roomCode = "";
                        config.appServer = "192.168.0.223";
                        config.appPort = 10002;
                        config.width = 1920;
                        config.height = 1080;
                        config.taskId = "123456";
                        config.debugTask = true;
                        config.appliType = AppliType_NV_AR;
                        xr_client_->Connect(config);
#endif
            }
        }

        // SendTouchDataToDataChannel
        if (xr_client_->media_ready()) {
            char buff[50];
            sprintf(buff, "{ type: %d, x: %f, y: %f }", JSON_COMMAND_TOUCH_UP, x, y);
            xr_client_->SendData(buff);
        }
    }
}

void ArApplication::SetScaleFactor(float scale) {
    float fovW = fov_w_ / scale * 0.5f;
    float fovH = fov_h_ / scale * 0.5f;
    float near_z = 0.01f;
    float far_z = 100.0f;
    float halfW = near_z * tanf( fovW * 3.14159260F / 180.0F );
    float halfH = near_z * tanf( fovH * 3.14159260F / 180.0F );

    glm::mat4 projection = glm::frustum(-halfW,halfW,-halfH,halfH, near_z, far_z);

    LOGV("getFov fovW=%f fovh=%f scale=%f", fovW, fovH, scale);

    lark::XRConfig::SetupFovWithProjectMatrix(LARKXR_EYE_LEFT, glm::value_ptr(projection));
    lark::XRConfig::SetupFovWithProjectMatrix(LARKXR_EYE_RIGHT, glm::value_ptr(projection));
}

void ArApplication::OnTrackingFrame(const larkxrTrackingFrame &trackingFrame) {
    trackingFrame_=trackingFrame;
}

void ArApplication::RequestTrackingInfo() {
    glm::mat4 raw_anchor;
    if (ar_manager_->GetRawAnchorPose(raw_anchor)) {
        xr_client_->SendArDevicePose(raw_anchor, ar_manager_->view_mat(), -rotation_radius_);
    }
}

void ArApplication::OnError(int errCode, const std::string &msg) {
    LOGE("on xr client error %d; msg %s;", errCode, msg.c_str());
    ArActivity_Error(msg.c_str());
}

void ArApplication::OnInfo(int infoCode, const std::string &msg) {
    LOGI("on xr client info %d; msg %s;", infoCode, msg.c_str());
    ArActivity_Toast(msg.c_str());
}

void ArApplication::OnConnected() {
    LOGI("on xr client OnConnected");
    ArActivity_Toast("连接成功，正在启动云端应用");
}

void ArApplication::OnClose(int code) {
    LOGI("on xr client OnClose %d", code );
    switch(code) {
        case LK_XR_MEDIA_TRANSPORT_CHANNEL_CLOSED:
            ArActivity_Error("与服务器媒体传输通道连接关闭");
            break;
        case LK_RENDER_SERVER_CLOSE:
            ArActivity_Error("与渲染服务器 TCP 连接关闭");
            break;
        case LK_PROXY_SERVER_CLOSE:
            ArActivity_Error("与渲染服务器代理连接关闭");
            break;
    }
}

void ArApplication::OnDataChannelOpen() {
    XRClientObserverWrap::OnDataChannelOpen();
    LOGI("OnDataChannelOpen");
}

void ArApplication::OnCloudXRReady(const std::string &appServerIp, const std::string &perferOutIp) {
    XRClientObserverWrap::OnCloudXRReady(appServerIp, perferOutIp);
    LOGI("OnCloudXRReady appServerIp %s perferOutIp %s", appServerIp.c_str(), perferOutIp.c_str());
#ifdef ENABLE_CLOUDXR
    prepare_public_ip_ = perferOutIp;
    cloudxr_client_->Connect(appServerIp);
//    cloudxr_client_->Connect("192.168.0.50");
#else
    LOGI("MUST ENABLE CLOUDXR AND PUT CloudXR SDK under projectroot/libs folder.");
#endif
}

#ifdef ENABLE_CLOUDXR
void ArApplication::UpdateClientState(cxrClientState state, cxrStateReason reason) {
    LOGI("UpdateClientState state %d reason %d", state, reason);
    switch (state) {
        case cxrClientState_ReadyToConnect:
            ArActivity_Toast("创建CloudXR客户端成功");
            break;
        case cxrClientState_ConnectionAttemptInProgress:
            ArActivity_Toast("开始连接服务器");
            break;
        case cxrClientState_StreamingSessionInProgress:
            ArActivity_Toast("连接服务器成功");
            break;
        case cxrClientState_ConnectionAttemptFailed:
        {
            if (!prepare_public_ip_.empty()) {
                need_reconnect_public_ip_ = true;
            } else {
                char buff[200];
                sprintf(buff, "连接CloudXR服务器失败 reason %d", reason);
                ArActivity_Error(buff);
            }
        }
            break;
        case cxrClientState_Disconnected:
        {
            char buff[200];
            sprintf(buff, "与CloudXR服务器连接断开 reason %d", reason);
            ArActivity_Error(buff);
        }
            break;
        default:
            break;
    }
}

void ArApplication::ReceiveUserData(const void *data, uint32_t size) {
    // ReceiveUserData here
    LOGI("cloudxr ReceiveUserData ");
}

void ArApplication::GetTrackingState(glm::mat4 *post_matrix) {
    // update cloudxr pose
    glm::mat4 anchor_pose_mat;
    ar_manager_->GetRawAnchorPose(anchor_pose_mat);
    glm::mat4 base_frame = glm::inverse(anchor_pose_mat);
    base_frame = glm::rotate(base_frame, -rotation_radius_, glm::vec3(0, 1, 0));
    // Setup pose matrix with our base frame
    *post_matrix = base_frame * glm::inverse(ar_manager_->view_mat());
}

#endif
