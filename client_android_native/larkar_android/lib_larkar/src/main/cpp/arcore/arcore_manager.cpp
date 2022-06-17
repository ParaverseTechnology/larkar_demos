//
// Created by fcx@pingxingyung.com on 2022/3/23.
// Copyright (c) 2022 www.pingxingyun.com All rights reserved.
//

#include <util.h>
#include "arcore_manager.h"

namespace {
    // TODO arcore utils
    glm::vec3 GetPlaneNormal(const ArSession& ar_session,
                             const ArPose& plane_pose) {
        float plane_pose_raw[7] = {0.f};
        ArPose_getPoseRaw(&ar_session, &plane_pose, plane_pose_raw);
        glm::quat plane_quaternion(plane_pose_raw[3], plane_pose_raw[0],
                                   plane_pose_raw[1], plane_pose_raw[2]);

        return plane_quaternion * (glm::vec3(0., 1.f, 0.));
    }

    float CalculateDistanceToPlane(const ArSession& ar_session,
                                   const ArPose& plane_pose,
                                   const ArPose& camera_pose) {
        float plane_pose_raw[7] = {0.f};
        ArPose_getPoseRaw(&ar_session, &plane_pose, plane_pose_raw);
        glm::vec3 plane_position(plane_pose_raw[4], plane_pose_raw[5],
                                 plane_pose_raw[6]);
        glm::vec3 normal = GetPlaneNormal(ar_session, plane_pose);

        float camera_pose_raw[7] = {0.f};
        ArPose_getPoseRaw(&ar_session, &camera_pose, camera_pose_raw);
        glm::vec3 camera_P_plane(camera_pose_raw[4] - plane_position.x,
                                 camera_pose_raw[5] - plane_position.y,
                                 camera_pose_raw[6] - plane_position.z);
        return glm::dot(normal, camera_P_plane);
    }

    void GetTransformMatrixFromAnchor(const ArAnchor& ar_anchor,
                                      ArSession* ar_session,
                                      glm::mat4* out_model_mat) {
        if (out_model_mat == nullptr) {
            LOGE("util::GetTransformMatrixFromAnchor model_mat is null.");
            return;
        }
        ArPose* pose = nullptr;
        ArPose_create(ar_session, nullptr, &pose);
        ArAnchor_getPose(ar_session, &ar_anchor, pose);
        ArPose_getMatrix(ar_session, pose, glm::value_ptr(*out_model_mat));
        ArPose_destroy(pose);
    }

    // Assumed distance from the device camera to the surface on which user will
    // try to place objects. This value affects the apparent scale of objects
    // while the tracking method of the Instant Placement point is
    // SCREENSPACE_WITH_APPROXIMATE_DISTANCE. Values in the [0.2, 2.0] meter
    // range are a good choice for most AR experiences. Use lower values for AR
    // experiences where users are expected to place objects on surfaces close
    // to the camera. Use larger values for experiences where the user will
    // likely be standing and trying to place an object on the ground or floor
    // in front of them.
    constexpr float kApproximateDistanceMeters = 1.0f;
}

ArcoreManager::ArcoreManager() = default;

void ArcoreManager::OnResume(JNIEnv *env, void *context, void *activity) {
    LOGI("ArcoreManager OnResume()");
    if (ar_session_ == nullptr) {
        CHECK(ArSession_create(env, context, &ar_session_) == AR_SUCCESS);

        // config
        const bool is_depth_supported = IsDepthSupported();

        ArConfig* ar_config = nullptr;
        ArConfig_create(ar_session_, &ar_config);
        if (is_depth_supported) {
            ArConfig_setDepthMode(ar_session_, ar_config, AR_DEPTH_MODE_AUTOMATIC);
        } else {
            ArConfig_setDepthMode(ar_session_, ar_config, AR_DEPTH_MODE_DISABLED);
        }

        // TODO setup instant_placement. disable for now.
        if (is_instant_placement_enabled_) {
            ArConfig_setInstantPlacementMode(ar_session_, ar_config,
                                             AR_INSTANT_PLACEMENT_MODE_LOCAL_Y_UP);
        } else {
            ArConfig_setInstantPlacementMode(ar_session_, ar_config,
                                             AR_INSTANT_PLACEMENT_MODE_DISABLED);
        }
        CHECK(ar_config);
        CHECK(ArSession_configure(ar_session_, ar_config) == AR_SUCCESS);
        ArConfig_destroy(ar_config);

        ArFrame_create(ar_session_, &ar_frame_);

        ArSession_setDisplayGeometry(ar_session_, display_rotation_, width_,
                                     height_);
    }

    const ArStatus status = ArSession_resume(ar_session_);
    CHECK(status == AR_SUCCESS);
}

void ArcoreManager::OnPause() {
    LOGI("ArcoreManager OnPause()");
    if (ar_session_ != nullptr) {
        ArSession_pause(ar_session_);
    }
}

void ArcoreManager::OnSurfaceCreated() {
    LOGI("ArcoreManager OnSurfaceCreated()");
    depth_texture_ = std::make_unique<ArcoreTexture>();
    depth_texture_->CreateOnGlThread();

    background_render_ = std::make_unique<ArcoreBackgroundRender>();
    background_render_->InitializeGlContent(depth_texture_->GetTextureId());

    plane_render_ = std::make_unique<ArcorePlaneRender>();
    plane_render_->InitializeGlContent();
}

void ArcoreManager::OnDisplayGeometryChanged(int display_rotation, int width, int height) {
    LOGI("ArcoreManager OnSurfaceChanged(%d, %d)", width, height);
    glViewport(0, 0, width, height);
    display_rotation_ = display_rotation;
    width_ = width;
    height_ = height;
    if (ar_session_ != nullptr) {
        ArSession_setDisplayGeometry(ar_session_, display_rotation, width, height);
    }
}

bool ArcoreManager::OnDrawFrame(bool drawPlane, bool depthColorVisualizationEnabled, bool useDepthForOcclusion) {
    // Render the scene.
    glClearColor(0.9f, 0.9f, 0.9f, 1.0f);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    // enable alpha
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    if (ar_session_ == nullptr) {
        return false;
    }

    ArSession_setCameraTextureName(ar_session_,background_render_->GetTextureId());

    // Update session to get current frame and render camera background.
    if (ArSession_update(ar_session_, ar_frame_) != AR_SUCCESS) {
        LOGE("HelloArApplication::OnDrawFrame ArSession_update error");
        return false;
    }

    // update camera
    ArCamera* ar_camera;
    ArFrame_acquireCamera(ar_session_, ar_frame_, &ar_camera);

    ArCamera_getViewMatrix(ar_session_, ar_camera, glm::value_ptr(view_mat_));
    ArCamera_getProjectionMatrix(ar_session_, ar_camera, 0.1f, 100.f, glm::value_ptr(project_mat_));

    background_render_->Draw(ar_session_, ar_frame_, depthColorVisualizationEnabled);

    ArTrackingState camera_tracking_state;
    ArCamera_getTrackingState(ar_session_, ar_camera, &camera_tracking_state);
    ArCamera_release(ar_camera);

    // update depth
    int32_t is_depth_supported = 0;
    ArSession_isDepthModeSupported(ar_session_, AR_DEPTH_MODE_AUTOMATIC,
                                   &is_depth_supported);
    if (is_depth_supported) {
        depth_texture_->UpdateWithDepthImageOnGlThread(*ar_session_, *ar_frame_);
    }

    // update light
    UpdateLight();

    // render planes
    if (drawPlane) {
        RenderPlanes();
    }

    return true;
}

bool ArcoreManager::HasDetectedPlanes() const {
    return plane_count_ > 0;
}

void ArcoreManager::OnDestory() {
    if (ar_session_ != nullptr) {
        ArSession_destroy(ar_session_);
        ArFrame_destroy(ar_frame_);
    }
    ReleaseChoosenAnchor();
    background_render_.reset();
    plane_render_.reset();
}

bool ArcoreManager::IsDepthSupported() {
    int32_t is_supported = 0;
    ArSession_isDepthModeSupported(ar_session_, AR_DEPTH_MODE_AUTOMATIC,
                                   &is_supported);
    return is_supported;
}

bool ArcoreManager::GetRawAnchorPose(glm::mat4 &anchorPose) {
    ArTrackingState trackingState = AR_TRACKING_STATE_STOPPED;
    ArAnchor_getTrackingState(ar_session_, anchor_, &trackingState);

    if (trackingState != AR_TRACKING_STATE_TRACKING) {
        return false;
    }
    GetTransformMatrixFromAnchor(*anchor_, ar_session_, &anchorPose);
    return true;
}

bool ArcoreManager::HitTest(float x, float y) {
    if (!ar_session_ || !ar_frame_) {
        return false;
    }

    ArHitResultList* hit_result_list = nullptr;
    ArHitResultList_create(ar_session_, &hit_result_list);
    CHECK(hit_result_list);
    if (is_instant_placement_enabled_) {
        ArFrame_hitTestInstantPlacement(ar_session_, ar_frame_, x, y,
                                        kApproximateDistanceMeters,
                                        hit_result_list);
    } else {
        ArFrame_hitTest(ar_session_, ar_frame_, x, y, hit_result_list);
    }

    int32_t hit_result_list_size = 0;
    ArHitResultList_getSize(ar_session_, hit_result_list,
                            &hit_result_list_size);

    // The hitTest method sorts the resulting list by distance from the camera,
    // increasing.  The first hit result will usually be the most relevant when
    // responding to user input.

    ArHitResult* ar_hit_result = nullptr;
    for (int32_t i = 0; i < hit_result_list_size; ++i) {
        ArHitResult* ar_hit = nullptr;
        ArHitResult_create(ar_session_, &ar_hit);
        ArHitResultList_getItem(ar_session_, hit_result_list, i, ar_hit);

        if (ar_hit == nullptr) {
            LOGE("HelloArApplication::OnTouched ArHitResultList_getItem error");
            return false;
        }

        ArTrackable* ar_trackable = nullptr;
        ArHitResult_acquireTrackable(ar_session_, ar_hit, &ar_trackable);
        ArTrackableType ar_trackable_type = AR_TRACKABLE_NOT_VALID;
        ArTrackable_getType(ar_session_, ar_trackable, &ar_trackable_type);
        // Creates an anchor if a plane or an oriented point was hit.
        if (AR_TRACKABLE_PLANE == ar_trackable_type) {
            ArPose* hit_pose = nullptr;
            ArPose_create(ar_session_, nullptr, &hit_pose);
            ArHitResult_getHitPose(ar_session_, ar_hit, hit_pose);
            int32_t in_polygon = 0;
            ArPlane* ar_plane = ArAsPlane(ar_trackable);
            ArPlane_isPoseInPolygon(ar_session_, ar_plane, hit_pose, &in_polygon);

            // Use hit pose and camera pose to check if hittest is from the
            // back of the plane, if it is, no need to create the anchor.
            ArPose* camera_pose = nullptr;
            ArPose_create(ar_session_, nullptr, &camera_pose);
            ArCamera* ar_camera;
            ArFrame_acquireCamera(ar_session_, ar_frame_, &ar_camera);
            ArCamera_getPose(ar_session_, ar_camera, camera_pose);
            ArCamera_release(ar_camera);
            float normal_distance_to_plane = CalculateDistanceToPlane(
                    *ar_session_, *hit_pose, *camera_pose);

            ArPose_destroy(hit_pose);
            ArPose_destroy(camera_pose);

            if (!in_polygon || normal_distance_to_plane < 0) {
                continue;
            }

            ar_hit_result = ar_hit;
            break;
        } else if (AR_TRACKABLE_POINT == ar_trackable_type) {
            ArPoint* ar_point = ArAsPoint(ar_trackable);
            ArPointOrientationMode mode;
            ArPoint_getOrientationMode(ar_session_, ar_point, &mode);
            if (AR_POINT_ORIENTATION_ESTIMATED_SURFACE_NORMAL == mode) {
                ar_hit_result = ar_hit;
                break;
            }
        } else if (AR_TRACKABLE_INSTANT_PLACEMENT_POINT == ar_trackable_type) {
            ar_hit_result = ar_hit;
        } else if (AR_TRACKABLE_DEPTH_POINT == ar_trackable_type) {
            // ArDepthPoints are only returned if ArConfig_setDepthMode() is called
            // with AR_DEPTH_MODE_AUTOMATIC.
            ar_hit_result = ar_hit;
        }
    }


    if (ar_hit_result) {
        // Note that the application is responsible for releasing the anchor
        // pointer after using it. Call ArAnchor_release(anchor) to release.
        ArAnchor* anchor = nullptr;
        if (ArHitResult_acquireNewAnchor(ar_session_, ar_hit_result, &anchor) !=
            AR_SUCCESS) {
            LOGE("HelloArApplication::OnTouched ArHitResult_acquireNewAnchor error");
            return false;
        }

        ArTrackingState tracking_state = AR_TRACKING_STATE_STOPPED;
        ArAnchor_getTrackingState(ar_session_, anchor, &tracking_state);
        if (tracking_state != AR_TRACKING_STATE_TRACKING) {
            ArAnchor_release(anchor);
            return false;
        }

        ArHitResult_destroy(ar_hit_result);
        ar_hit_result = nullptr;

        ArHitResultList_destroy(hit_result_list);
        hit_result_list = nullptr;

        // save anchor
        anchor_ = anchor;
    }
    LOGI("arcore hit success %f %f", x, y);
    return true;
}

bool ArcoreManager::ReleaseChoosenAnchor() {
    if (!anchor_) {
        return false;
    }
    ArAnchor_release(anchor_);
    anchor_ = nullptr;
    return false;
}

bool ArcoreManager::HasChoosenAnchor() {
    return anchor_ != nullptr;
}

void ArcoreManager::RenderPlanes() {
    // Update and render planes.
    ArTrackableList* plane_list = nullptr;
    ArTrackableList_create(ar_session_, &plane_list);
    CHECK(plane_list != nullptr);

    ArTrackableType plane_tracked_type = AR_TRACKABLE_PLANE;
    ArSession_getAllTrackables(ar_session_, plane_tracked_type, plane_list);

    int32_t plane_list_size = 0;
    ArTrackableList_getSize(ar_session_, plane_list, &plane_list_size);
    plane_count_ = plane_list_size;

    for (int i = 0; i < plane_list_size; ++i) {
        ArTrackable* ar_trackable = nullptr;
        ArTrackableList_acquireItem(ar_session_, plane_list, i, &ar_trackable);
        ArPlane* ar_plane = ArAsPlane(ar_trackable);
        ArTrackingState out_tracking_state;
        ArTrackable_getTrackingState(ar_session_, ar_trackable,
                                     &out_tracking_state);

        ArPlane* subsume_plane;
        ArPlane_acquireSubsumedBy(ar_session_, ar_plane, &subsume_plane);
        if (subsume_plane != nullptr) {
            ArTrackable_release(ArAsTrackable(subsume_plane));
            ArTrackable_release(ar_trackable);
            continue;
        }

        if (ArTrackingState::AR_TRACKING_STATE_TRACKING != out_tracking_state) {
            ArTrackable_release(ar_trackable);
            continue;
        }

        plane_render_->Draw(project_mat_, view_mat_, *ar_session_, *ar_plane);
        ArTrackable_release(ar_trackable);
    }

    ArTrackableList_destroy(plane_list);
    plane_list = nullptr;
}

void ArcoreManager::UpdateLight() {
    // Get light estimation value.
    ArLightEstimate* ar_light_estimate;
    ArLightEstimateState ar_light_estimate_state;
    ArLightEstimate_create(ar_session_, &ar_light_estimate);

    ArFrame_getLightEstimate(ar_session_, ar_frame_, ar_light_estimate);
    ArLightEstimate_getState(ar_session_, ar_light_estimate,
                             &ar_light_estimate_state);

    // Set light intensity to default. Intensity value ranges from 0.0f to 1.0f.
    // The first three components are color scaling factors.
    // The last one is the average pixel intensity in gamma space.
    float color_correction[4] = {1.f, 1.f, 1.f, 1.f};
    if (ar_light_estimate_state == AR_LIGHT_ESTIMATE_STATE_VALID) {
        ArLightEstimate_getColorCorrection(ar_session_, ar_light_estimate,
                                           color_correction);
    }

    ArLightEstimate_destroy(ar_light_estimate);
    ar_light_estimate = nullptr;
}

bool ArcoreManager::Set60FPS() {
    if (!ar_session_) {
        LOGE("set arcore fps without ar session");
        return false;
    }
    // Retrieve supported camera configs.
    ArCameraConfigList* all_camera_configs = nullptr;
    int32_t num_configs = 0;
    ArCameraConfigList_create(ar_session_, &all_camera_configs);
    // Create filter first to get both 30 and 60 fps.
    ArCameraConfigFilter* camera_config_filter = nullptr;
    ArCameraConfigFilter_create(ar_session_, &camera_config_filter);
    ArCameraConfigFilter_setTargetFps(
            ar_session_, camera_config_filter,
            AR_CAMERA_CONFIG_TARGET_FPS_60);
    ArSession_getSupportedCameraConfigsWithFilter(
            ar_session_, camera_config_filter, all_camera_configs);
    ArCameraConfigList_getSize(ar_session_, all_camera_configs, &num_configs);

    if (num_configs < 1) {
        LOGW("No 60Hz camera available!  Setting to 30fps.");
        return false;
    } else {
        ArCameraConfig* camera_config;
        ArCameraConfig_create(ar_session_, &camera_config);
        ArCameraConfigList_getItem(ar_session_, all_camera_configs, 0,
                                   camera_config);

        ArSession_setCameraConfig(ar_session_, camera_config);
    }
    LOGI("set 60hz camera success.");
    return true;
}
