//
// Created by fcx@pingxingyung.com on 2022/3/23.
// Copyright (c) 2022 www.pingxingyun.com All rights reserved.
//

#ifndef LARKXR_AR_MANAGER_INTERFACE_H
#define LARKXR_AR_MANAGER_INTERFACE_H

#include "jni.h"
#include "glm/glm.hpp"

class ArManagerInterface {
public:
    ArManagerInterface() = default;
    virtual ~ArManagerInterface() = default;

    // life cycle
    virtual void OnResume(JNIEnv* env, void* context, void* activity) = 0;
    virtual void OnPause() = 0;
    virtual void OnSurfaceCreated() = 0;
    virtual void OnDisplayGeometryChanged(int display_rotation, int width, int height) = 0;
    virtual bool OnDrawFrame(bool drawPlane,
                             bool depthColorVisualizationEnabled = false,
                             bool useDepthForOcclusion = false) = 0;
    virtual bool HasDetectedPlanes() const = 0;
    virtual void OnDestory() = 0;

    // anchor
    virtual bool HitTest(float x, float y) = 0;
    virtual bool ReleaseChoosenAnchor() = 0;
    virtual bool HasChoosenAnchor() = 0;
    virtual bool GetRawAnchorPose(glm::mat4& anchorPose) = 0;

    inline const glm::mat4& raw_anchor_mat() const { return raw_anchor_mat_; };
    inline const glm::mat4& view_mat() const { return view_mat_; };
    inline const glm::mat4& project_mat() const { return project_mat_; };

    inline const int width() { return width_; };
    inline const int height() { return height_; };
protected:
    int display_rotation_ = 0;
    int width_ = 1920;
    int height_ = 1080;

    glm::mat4 raw_anchor_mat_;
    glm::mat4 view_mat_;
    glm::mat4 project_mat_;
};

#endif //LARKXR_AR_MANAGER_INTERFACE_H
