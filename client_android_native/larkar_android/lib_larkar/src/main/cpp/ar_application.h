//
// Created by Hayasi-Yumito on 2021/12/14.
//

#ifndef LARKXR_AR_APPLICATION_H
#define LARKXR_AR_APPLICATION_H

#include "lark_xr/xr_client.h"
#include "ar_manager_interface.h"

// SETUP SDKID
#define LARK_SDK_ID "29b8347a75cc44898fc312e177a857af"

class ArApplication: public lark::XRClientObserverWrap {
public:
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
    virtual void OnConnected() override;
    virtual void OnClose(int code) override;
    virtual void RequestTrackingInfo() override;
    virtual void OnTrackingFrame(const larkxrTrackingFrame& trackingFrame) override;
    virtual void OnInfo(int infoCode, const std::string& msg) override;
    virtual void OnError(int errCode, const std::string& msg) override;
    virtual void OnDataChannelOpen() override;

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
};


#endif //LARKXR_AR_APPLICATION_H
