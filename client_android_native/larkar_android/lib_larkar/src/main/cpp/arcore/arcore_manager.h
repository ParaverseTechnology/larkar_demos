//
// Created by fcx@pingxingyung.com on 2022/3/23.
// Copyright (c) 2022 www.pingxingyun.com All rights reserved.
//

#ifndef LARKXR_ARCORE_MANAGER_H
#define LARKXR_ARCORE_MANAGER_H

#include <arcore_c_api.h>
#include "ar_manager_interface.h"
#include "huawei_arengine_interface.h"
#include "arcore_plane_render.h"
#include "arcore_background_render.h"
#include "arcore_texture.h"

class ArcoreManager: public ArManagerInterface {
public:
    ArcoreManager();

    virtual void OnResume(JNIEnv* env, void* context, void* activity) override;
    virtual void OnPause() override;
    virtual void OnSurfaceCreated() override;
    virtual void OnDisplayGeometryChanged(int display_rotation, int width, int height) override;
    virtual bool OnDrawFrame(bool drawPlane,
                             bool depthColorVisualizationEnabled = false,
                             bool useDepthForOcclusion = false) override;
    virtual bool HasDetectedPlanes() const override;
    virtual void OnDestory() override;

    // anchor
    virtual bool HitTest(float x, float y) override;
    virtual bool ReleaseChoosenAnchor() override;
    virtual bool HasChoosenAnchor() override;
    // fps config
    virtual bool Set60FPS() override;

    virtual bool GetRawAnchorPose(glm::mat4& anchorPose);
private:
    // Returns true if depth is supported.
    bool IsDepthSupported();
    void RenderPlanes();
    void UpdateLight();

    ArSession* ar_session_ = nullptr;
    ArFrame* ar_frame_ = nullptr;
    ArAnchor* anchor_ = nullptr;

    // TODO setup instant_placement. disable for now.
    bool is_instant_placement_enabled_ = false;

    std::unique_ptr<ArcorePlaneRender> plane_render_ = nullptr;
    std::unique_ptr<ArcoreBackgroundRender> background_render_ = nullptr;
    std::unique_ptr<ArcoreTexture> depth_texture_ = nullptr;

    int plane_count_ = 0;
};


#endif //LARKXR_ARCORE_MANAGER_H
