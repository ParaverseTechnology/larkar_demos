//
// Created by fcx@pingxingyung.com on 2022/3/23.
// Copyright (c) 2022 www.pingxingyun.com All rights reserved.
//

#include <util.h>
#include "arengine_manager.h"

ArEngineManager::ArEngineManager() = default;

void ArEngineManager::OnResume(JNIEnv *env, void *context, void *activity) {
    LOGI("ArEngineManager::OnResume()");
    if (ar_session_ == nullptr) {
        CHECK(HwArSession_create(env, context, &ar_session_) == HWAR_SUCCESS);
        CHECK(ar_session_);

        HwArConfig *arConfig = nullptr;
        HwArConfig_create(ar_session_, &arConfig);

        CHECK(HwArSession_configure(ar_session_, arConfig) == HWAR_SUCCESS);

        HwArConfig_destroy(arConfig);
        HwArFrame_create(ar_session_, &ar_frame_);
        HwArSession_setDisplayGeometry(ar_session_, display_rotation_, width_, height_);
    }
    const HwArStatus status = HwArSession_resume(ar_session_);
    CHECK(status == HWAR_SUCCESS);
}

void ArEngineManager::OnPause() {
    if (ar_session_ != nullptr) {
        HwArSession_pause(ar_session_);
    }
}

void ArEngineManager::OnSurfaceCreated() {
    LOGI("ArEngineManager-----Initialize() start.");
    background_render_ = std::make_unique<ArengineBackgroundRender>();
    plane_render_ = std::make_unique<ArenginePlaneRender>();

    background_render_->InitializeBackGroundGlContent();
    plane_render_->InitializePlaneGlContent();
    LOGI("ArEngineManager-----Initialize() stop.");
}

void ArEngineManager::OnDisplayGeometryChanged(int display_rotation, int width, int height) {
    LOGI("ArEngineManager::OnDisplayGeometryChanged(%d, %d)", width, height);
    glViewport(0, 0, width, height);
    display_rotation_ = display_rotation;
    if (width > height) {
        width_ = width;
        height_ = height;
    } else {
        width_ = height;
        height_ = width;
    }
    if (ar_session_ != nullptr) {
        HwArSession_setDisplayGeometry(ar_session_, display_rotation, width, height);
    }
}

bool ArEngineManager::OnDrawFrame(bool drawPlane, bool depthColorVisualizationEnabled, bool useDepthForOcclusion) {
    if (!InitializeDraw()) {
        return false;
    }
    if (drawPlane) {
        RenderPlanes();
    }
    return true;
}

bool ArEngineManager::HasDetectedPlanes() const {
    return plane_status_;
}

void ArEngineManager::OnDestory() {
    if (ar_session_ != nullptr) {
        HwArSession_destroy(ar_session_);
        HwArFrame_destroy(ar_frame_);
    }
    ReleaseChoosenAnchor();
    background_render_.reset();
    plane_render_.reset();
}

bool ArEngineManager::InitializeDraw() {
    // Render the scene.
    glClearColor(0.9f, 0.9f, 0.9f, 1.0f);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    if (ar_session_ == nullptr) {
        LOGE("arSessionNull");
        return false;
    }

    HwArSession_setCameraTextureName(ar_session_, background_render_->GetTextureId());

    // HwArSession update: Obtains the latest HwArFrame.
    if (HwArSession_update(ar_session_, ar_frame_) != HWAR_SUCCESS) {
        LOGE("WorldRenderManager::InitializeDraw ArSession_update error");
    }

    HwArCamera *arCamera;
    HwArFrame_acquireCamera(ar_session_, ar_frame_, &arCamera);
    HwArCamera_getViewMatrix(ar_session_, arCamera, glm::value_ptr(view_mat_));

    // Near (0.1) Far (100).
    HwArCamera_getProjectionMatrix(ar_session_, arCamera, 0.1f, 100.f,
                                   glm::value_ptr(project_mat_));
    HwArTrackingState cameraTrackingState = HWAR_TRACKING_STATE_STOPPED;
    HwArCamera_getTrackingState(ar_session_, arCamera, &cameraTrackingState);

    HwArCamera_release(arCamera);

    background_render_->Draw(ar_session_, ar_frame_);

    // If the camera is not in tracking state, the current frame is not drawn.
    return cameraTrackingState == HWAR_TRACKING_STATE_TRACKING;
}

void ArEngineManager::RenderPlanes() {
    // Update and render the plane.
    HwArTrackableList *planeList = nullptr;
    HwArTrackableList_create(ar_session_, &planeList);
    CHECK(planeList != nullptr);

    HwArTrackableType planeTrackedType = HWAR_TRACKABLE_PLANE;
    HwArSession_getAllTrackables(ar_session_, planeTrackedType, planeList);

    HwArTrackableList_getSize(ar_session_, planeList, &plane_count_);

    for (int i = 0; i < plane_count_; ++i) {
        HwArTrackable *arTrackable = nullptr;
        HwArTrackableList_acquireItem(ar_session_, planeList, i, &arTrackable);
        HwArPlane *arPlane = HwArAsPlane(arTrackable);
        HwArTrackingState outTrackingState;
        HwArTrackable_getTrackingState(ar_session_, arTrackable,
                                       &outTrackingState);

        HwArPlane *subsumePlane = nullptr;
        HwArPlane_acquireSubsumedBy(ar_session_, arPlane, &subsumePlane);

        if (subsumePlane != nullptr) {
            HwArTrackable_release(HwArAsTrackable(subsumePlane));
            continue;
        }

        if (HwArTrackingState::HWAR_TRACKING_STATE_TRACKING != outTrackingState) {
            continue;
        }

        HwArTrackingState planeTrackingState;
        HwArTrackable_getTrackingState(ar_session_, HwArAsTrackable(arPlane),
                                       &planeTrackingState);
        if (planeTrackingState == HWAR_TRACKING_STATE_TRACKING) {
            glm::vec3 color;
            RendererPlane(arPlane, arTrackable, color);
            plane_render_->Draw(project_mat_, view_mat_, ar_session_, arPlane, color);
            plane_status_ = true;
        }
    }

    HwArTrackableList_destroy(planeList);
    planeList = nullptr;
}

void
ArEngineManager::RendererPlane(HwArPlane *arPlane, HwArTrackable *arTrackable, glm::vec3 &color) {
    const auto iter = plane_color_map_.find(arPlane);
    if (iter != plane_color_map_.end()) {
        color = iter->second;
        HwArTrackable_release(arTrackable);
    } else {
        // Set the plane color. The first plane is white, and the other planes are blue.
        if (!first_plane_has_been_found_) {
            first_plane_has_been_found_ = true;
            color = {255, 255, 255};
        } else {
            color = {0, 206, 209};
        }
        plane_color_map_.insert(std::pair<HwArPlane *, glm::vec3>(arPlane, color));
    }
}

bool ArEngineManager::HitTest(float x, float y) {
    if (!ar_session_ || !ar_frame_) {
        return false;
    }
    HwArHitResultList *hitResultList = nullptr;
    HwArHitResultList_create(ar_session_, &hitResultList);
    CHECK(hitResultList);
    HwArFrame_hitTest(ar_session_, ar_frame_, x, y, hitResultList);

    int32_t hitResultListSize = 0;
    HwArHitResultList_getSize(ar_session_, hitResultList, &hitResultListSize);

    // The hitTest method sorts the result list by the distance to the camera in ascending order.
    // When responding to user input, the first hit result is usually most relevant.
    HwArHitResult *arHitResult = nullptr;
    HwArTrackableType trackableType = HWAR_TRACKABLE_NOT_VALID;
    bool hasHitFlag = false;

    if (!GetHitResult(arHitResult, hasHitFlag, hitResultListSize, trackableType,
                      hitResultList)) {
        LOGI("hit failed");
        return false;
    }
    if (!hasHitFlag) {
        LOGI("hit success");
        return false;
    }
    if (arHitResult) {
        // Note that the app should release the anchor_ pointer after using it.
        // Call ArAnchor_release(anchor_) to release the anchor_.
        if (HwArHitResult_acquireNewAnchor(ar_session_, arHitResult, &anchor_) !=
            HWAR_SUCCESS) {
            LOGE("HitTest ArHitResult_acquireNewAnchor error");
            return false;
        }

        HwArTrackingState trackingState = HWAR_TRACKING_STATE_STOPPED;
        HwArAnchor_getTrackingState(ar_session_, anchor_, &trackingState);
        if (trackingState != HWAR_TRACKING_STATE_TRACKING) {
            HwArAnchor_release(anchor_);
            LOGE("HitTest tracking state %d", trackingState);
            return false;
        }
        HwArHitResult_destroy(arHitResult);
        arHitResult = nullptr;

        HwArHitResultList_destroy(hitResultList);
        hitResultList = nullptr;
        return true;
    }
    return false;
}

bool ArEngineManager::ReleaseChoosenAnchor() {
    if (!anchor_) {
        return false;
    }
    HwArAnchor_release(anchor_);
    anchor_ = nullptr;
    return true;
}

bool ArEngineManager::HasChoosenAnchor() {
    return anchor_ != nullptr;
}

bool ArEngineManager::GetHitResult(HwArHitResult *&arHitResult, bool &hasHitFlag,
                                   int32_t hitResultListSize, HwArTrackableType &trackableType,
                                   HwArHitResultList *hitResultList) {
    for (int32_t i = 0; i < hitResultListSize; ++i) {
        HwArHitResult *arHit = nullptr;
        HwArHitResult_create(ar_session_, &arHit);
        HwArHitResultList_getItem(ar_session_, hitResultList, i, arHit);

        if (arHit == nullptr) {
            return false;
        }

        HwArTrackable *arTrackable = nullptr;
        HwArHitResult_acquireTrackable(ar_session_, arHit, &arTrackable);
        HwArTrackableType ar_trackable_type = HWAR_TRACKABLE_NOT_VALID;
        HwArTrackable_getType(ar_session_, arTrackable, &ar_trackable_type);

        // If a plane or directional point is encountered, an anchor_ point is created.
        if (HWAR_TRACKABLE_PLANE == ar_trackable_type) {
            HwArPose *arPose = nullptr;
            HwArPose_create(ar_session_, nullptr, &arPose);
            HwArHitResult_getHitPose(ar_session_, arHit, arPose);
            int32_t inPolygon = 0;
            HwArPlane *arPlane = HwArAsPlane(arTrackable);
            HwArPlane_isPoseInPolygon(ar_session_, arPlane, arPose, &inPolygon);

            // Use the hit pose and camera pose to check whether the hit position comes from the back of the plane.
            // If yes, no anchor_ needs to be created.
            HwArPose *cameraPose = nullptr;
            HwArPose_create(ar_session_, nullptr, &cameraPose);
            HwArCamera *arCamera;
            HwArFrame_acquireCamera(ar_session_, ar_frame_, &arCamera);
            HwArCamera_getPose(ar_session_, arCamera, cameraPose);
            HwArCamera_release(arCamera);
            float normal_distance_to_plane = gWorldAr::util::CalculateDistanceToPlane(*ar_session_,
                                                                                      *arPose,
                                                                                      *cameraPose);

            HwArPose_destroy(arPose);
            HwArPose_destroy(cameraPose);
            if (!inPolygon || normal_distance_to_plane < 0) {
                continue;
            }

            arHitResult = arHit;
            trackableType = ar_trackable_type;
            hasHitFlag = true;
            break;
        } else if (HWAR_TRACKABLE_POINT == ar_trackable_type) {
            HwArPoint *ar_point = HwArAsPoint(arTrackable);
            HwArPointOrientationMode mode;
            HwArPoint_getOrientationMode(ar_session_, ar_point, &mode);
            if (HWAR_POINT_ORIENTATION_ESTIMATED_SURFACE_NORMAL == mode) {
                arHitResult = arHit;
                trackableType = ar_trackable_type;
                hasHitFlag = true;
            }
        }
    }
    return true;
}

bool ArEngineManager::GetRawAnchorPose(glm::mat4 &anchorPose) {
    //
    HwArTrackingState trackingState = HWAR_TRACKING_STATE_STOPPED;
    HwArAnchor_getTrackingState(ar_session_, anchor_, &trackingState);

    if (trackingState != HWAR_TRACKING_STATE_TRACKING) {
        return false;
    }

    gWorldAr::util::GetTransformMatrixFromAnchor(ar_session_, anchor_, &anchorPose);
    return true;
}

