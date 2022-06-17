//
// Created by fcx@pingxingyung.com on 2022/6/6.
// Copyright (c) 2022 www.pingxingyun.com All rights reserved.
//

#ifndef LARKAR_CLOUDXR_CLIENT_H
#define LARKAR_CLOUDXR_CLIENT_H

#ifdef ENABLE_CLOUDXR
#include "CloudXRClient.h"
#include "CloudXRInputEvents.h"
#include "CloudXRClientOptions.h"
#include "CloudXRMatrixHelpers.h"
#endif
#include "util.h"


class CloudXRLaunchOptions : public CloudXR::ClientOptions {
public:
    bool using_env_lighting_;
    float res_factor_;

    CloudXRLaunchOptions();
};

class CloudXRClientObserver {
public:
    virtual void UpdateClientState(cxrClientState state, cxrStateReason reason) = 0;
    virtual void ReceiveUserData(const void* data, uint32_t size) = 0;
    virtual void GetTrackingState(glm::mat4* post_matrix) = 0;
};

class CloudXRClient {
public:
    CloudXRClient(CloudXRClientObserver* ob);
    ~CloudXRClient();

    // CloudXR interface callbacks
    void TriggerHaptic(const cxrHapticFeedback*) {}
    void GetTrackingState(cxrVRTrackingState* state);
    cxrBool RenderAudio(const cxrAudioFrame *audioFrame);
    void ReceiveUserData(const void* data, uint32_t size);
    void UpdateClientState(cxrClientState state, cxrStateReason reason);

    cxrDeviceDesc GetDeviceDesc();
    bool Init();
    cxrError Connect(const std::string& serverIp);
    void Teardown();
    void SetProjectionMatrix(const glm::mat4& projection);
    void SetFps(int fps);
    int DetermineOffset() const;
    cxrError Latch();
    void Release();
    void Render(const float color_correction[4]);
    void Stats();
    void UpdateLightProps(const float primaryDirection[3], const float primaryIntensity[3],
                          const float ambient_spherical_harmonics[27]);
    bool HandleLaunchOptions(std::string &cmdline);

    // this is used to tell the client what the display/surface resolution is.
    // here, we can apply a factor to reduce what we tell the server our desired
    // video resolution should be.
    void SetStreamRes(uint32_t w, uint32_t h, uint32_t orientation);
    // Send a touch event along to the server/host application
    void HandleTouch(float x, float y);

    inline bool IsRunning() const { return cloudxr_receiver_; };

    inline void SetArgs(const std::string &args) {
        launch_options_.ParseString(args);
    };

    inline std::string GetServerAddr() {
        return launch_options_.mServerIP;
    };

    inline const CloudXRLaunchOptions& GetLaunchOptions() {
        return launch_options_;
    };

    inline bool GetUseEnvLighting() {
        return launch_options_.using_env_lighting_;
    };

    inline bool IsConnectStarted() {
        return client_state_ == cxrClientState_ConnectionAttemptInProgress || client_state_ == cxrClientState_StreamingSessionInProgress;
    }

    inline bool IsConnect() {
        return client_state_ == cxrClientState_StreamingSessionInProgress;
    };
private:
    static constexpr int kQueueLen = 16;

    void SetPoseMatrix(const glm::mat4& pose_mat);

    cxrReceiverHandle cloudxr_receiver_ = nullptr;

    CloudXRLaunchOptions launch_options_;

    uint32_t stream_width_ = 1698;
    uint32_t stream_height_ = 754;

    cxrFramesLatched framesLatched_ = {};
    bool latched_ = false;

    std::mutex state_mutex_;
    cxrMatrix34 pose_matrix_[kQueueLen] = {};
    cxrDeviceDesc device_desc_ = {};
    int current_idx_ = 0;

    int fps_ = 60;

    cxrConnectionStats stats_ = {};
    int frames_until_stats_ = 60;

    cxrClientState client_state_ = cxrClientState_ReadyToConnect;

    CloudXRClientObserver* ob_;
};


#endif //LARKAR_CLOUDXR_CLIENT_H
