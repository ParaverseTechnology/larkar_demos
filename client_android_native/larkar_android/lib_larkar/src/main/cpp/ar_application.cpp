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
}

ArApplication::ArApplication(jobject j_act, JNIEnv *Env, const std::string& appid, int sdkType):
    appid_(appid),
    ar_sdk_type_(sdkType)
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
    LOGI("WorldArApplication::OnSurfaceCreated()");
    ar_manager_->OnSurfaceCreated();
    LOGD("ArApplication::InitXr()");
    xr_client_ = std::make_shared<lark::XRClient>();
    xr_client_->Init(nullptr);
    xr_client_->RegisterObserver(this);
    LOGD("url %s:%hu", lark::XRClient::GetServerHost().c_str(), lark::XRClient::GetServerPort());
    // xr_client_->SetServerAddr(lark::XRClient::GetServerHost(), lark::XRClient::GetServerPort());
//     xr_client_->SetServerAddr("192.168.0.161", 8181);
//     xr_client_->SetServerAddr("192.168.0.55", 8181);
//    xr_client_->SetServerAddr("192.168.0.177", 8181);

    if (!xr_client_->InitSdkAuthorization(LARK_SDK_ID)) {
        LOGE("init sdk auth faild %d %s", xr_client_->last_error_code(), xr_client_->last_error_message().c_str());
    }
}

void ArApplication::OnDisplayGeometryChanged(int displayRotation, int width, int height) {
    LOGI("WorldArApplication::OnDisplayGeometryChanged(%d, %d)", width, height);
    glViewport(0, 0, width, height);
    display_rotation_ = displayRotation;
    if (width > height) {
        width_ = width;
        height_ = height;
    } else {
        width_ = height;
        height_ = width;
    }

    ar_manager_->OnDisplayGeometryChanged(displayRotation, width, height);

    lark::XRConfig::use_multiview = false;
    // 显示参数要跟手机上对应好
    // 手机刷新率为 60
    lark::XRConfig::fps = 60;
    LOGD("width-height^%d^%d", width_, height_);
    // 分辨率要跟手机上一样
    lark::XRConfig::render_width = 1280;
    //lark::XRConfig::render_width = width_;
    lark::XRConfig::render_height = 720;

    lark::XRConfig::QuickConfigWithDefaulSetup(lark::QuickConfigLevel_Fast);

    // setup fov
    glm::mat4 projection = ar_manager_->project_mat();
    lark::XRConfig::SetupFovWithProjectMatrix(LARKXR_EYE_LEFT, glm::value_ptr(projection));
    lark::XRConfig::SetupFovWithProjectMatrix(LARKXR_EYE_RIGHT, glm::value_ptr(projection));
    fov_w_ = lark::XRConfig::fov[0].left + lark::XRConfig::fov[0].right;
    fov_h_ = lark::XRConfig::fov[0].top + lark::XRConfig::fov[0].bottom;
    LOGI("default getFov w=%f h=%f", fov_w_, fov_h_);
}


void ArApplication::OnDrawFrame() {
//    LOGI("ArApplication::OnDrawFrame()");
    // latency
    lark::XRLatencyCollector::Instance().Rendered2(trackingFrame_.frameIndex);

    bool res = ar_manager_->OnDrawFrame(!ar_manager_->HasChoosenAnchor(), false);

    if (res && ar_manager_->HasChoosenAnchor()) {
        xr_client_->Draw();
    }

    lark::XRLatencyCollector::Instance().Submit(trackingFrame_.frameIndex, 0);
}

void ArApplication::OnTouched(float x, float y, jboolean longtap) {
    LOGD("OnTouched x:%f y:%f long:%d; choosen %d", x, y, longtap, ar_manager_->HasChoosenAnchor() );
    if (longtap) {
        ar_manager_->ReleaseChoosenAnchor();
    } else {
        if (!ar_manager_->HasChoosenAnchor() && ar_manager_->HitTest(x, y)) {
            if (!xr_client_->is_connected()) {
#if 1
                LOGD("EnterAppli %s", appid_.c_str());
                xr_client_->EnterAppli(appid_);
#else
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
                        xr_client_->Connect(config);
#endif
            }
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
        xr_client_->SendArDevicePose(raw_anchor, ar_manager_->view_mat(), rotation_radius_);
    }
}

void ArApplication::OnError(int errCode, const std::string &msg) {
    LOGE("on xr client error %d; msg %s;", errCode, msg.c_str());
    ArActivity_Toast(msg.c_str());
}
