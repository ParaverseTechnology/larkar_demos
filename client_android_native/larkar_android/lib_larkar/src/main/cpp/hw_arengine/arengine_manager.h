//
// Created by fcx@pingxingyung.com on 2022/3/23.
// Copyright (c) 2022 www.pingxingyun.com All rights reserved.
//

#ifndef LARKXR_ARENGINE_MANAGER_H
#define LARKXR_ARENGINE_MANAGER_H

#include <unordered_map>
#include "ar_manager_interface.h"
#include "huawei_arengine_interface.h"
#include "arengine_background_render.h"
#include "arengine_plane_render.h"

class ArEngineManager: public ArManagerInterface {
public:
    ArEngineManager();

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

    virtual bool GetRawAnchorPose(glm::mat4& anchorPose) override;
private:
    bool InitializeDraw();
    void RenderPlanes();
    void RendererPlane(HwArPlane *arPlane, HwArTrackable *arTrackable, glm::vec3 &color);
    bool GetHitResult(HwArHitResult *&arHitResult, bool &hasHitFlag, int32_t hitResultListSize,
                      HwArTrackableType &trackableType, HwArHitResultList *hitResultList);

    HwArSession* ar_session_ = nullptr;
    HwArFrame* ar_frame_ = nullptr;
    HwArAnchor* anchor_ = nullptr;

    std::unique_ptr<ArengineBackgroundRender> background_render_ = nullptr;
    std::unique_ptr<ArenginePlaneRender> plane_render_ = nullptr;

    int plane_count_ = 0;
    bool plane_status_ = false;
    bool first_plane_has_been_found_ = false;
    // Store the randomly selected colors used by each plane.
    std::unordered_map<HwArPlane *, glm::vec3> plane_color_map_ = {};
};


#endif //LARKXR_ARENGINE_MANAGER_H
