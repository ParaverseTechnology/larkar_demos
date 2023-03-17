//
// Created by Hayasi-Yumito on 2021/12/14.
//

#ifndef LARKXR_AR_APPLICATION_H
#define LARKXR_AR_APPLICATION_H

#include "lark_xr/xr_client.h"
#include "ar_manager_interface.h"
#include "oboe/Oboe.h"

#ifdef ENABLE_CLOUDXR
#include "cloudxr_client.h"
#endif

// #define LARK_SDK_ID "如果没有 SDK 授权码，联系 business@pingxingyun.com 获取,注意是 SDK 本身的授权码，不是服务器上的授权"
#ifndef LARK_SDK_ID
// 请将 SDK ID 填入第 16 行 LARK_SDK_ID 中，并放开第 16 行注释
#error "请将 SDK ID 填入第 16 行 LARK_SDK_ID 中，并放开第 16 行注释;如果没有 SDK 授权码，联系 business@pingxingyun.com 获取,注意是 SDK 本身的授权码，不是服务器上的授权"
#endif

class ArApplication:
        public lark::XRClientObserverWrap
        , public oboe::AudioStreamDataCallback
#ifdef ENABLE_CLOUDXR
    ,public CloudXRClientObserver
#endif
{
public:
    const uint32_t AUDIO_CHANNEL_COUNT = 2;             ///< Audio is currently always stereo
    const uint32_t AUDIO_SAMPLE_SIZE = sizeof(int16_t); ///< Audio is currently singed 16-bit samples (little-endian)
    const uint32_t AUDIO_SAMPLING_RATE = 48000;         ///< Audio is currently always 48khz

    ArApplication(const std::string& appid, int sdkType);
    ~ArApplication();

    void OnResume(void* env, void* context, void* activity);
    void OnPause();

    void OnDisplayGeometryChanged(int displayRotation, int width, int height);
    void OnDrawFrame();
    void OnSurfaceCreated();
    // OnTouched is called on the OpenGL thread after the user touches the screen.
    // @param x: x position on the screen (pixels).
    // @param y: y position on the screen (pixels).
    // @param longPress: a long press occured.
    void OnTouched(float x, float y,jboolean longtap);

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

    inline void set_rotation_radius(float radius) { rotation_radius_ = radius; };

    void SetScaleFactor(float scalefactor);
private:
    int width_ = 1;
    int height_ = 1;

    larkxrTrackingFrame trackingFrame_;

    std::string appid_;

    std::shared_ptr<lark::XRClient> xr_client_ = nullptr;

    float rotation_radius_ = 0;

    float fov_w_ = 45.0f;
    float fov_h_ = 45.0f;

    std::unique_ptr<ArManagerInterface> ar_manager_ = nullptr;

    bool support_datachannel_ = false;

    std::shared_ptr<oboe::AudioStream> recording_stream_{};
#ifdef ENABLE_CLOUDXR
    std::unique_ptr<CloudXRClient> cloudxr_client_ = nullptr;
    std::string prepare_public_ip_ = "";
    bool need_reconnect_public_ip_ = false;
#endif
};


#endif //LARKXR_AR_APPLICATION_H
