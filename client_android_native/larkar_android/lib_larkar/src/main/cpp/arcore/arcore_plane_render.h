//
// Created by fcx@pingxingyung.com on 2022/3/24.
// Copyright (c) 2022 www.pingxingyun.com All rights reserved.
//

#ifndef LARKXR_ARCORE_PLANE_RENDER_H
#define LARKXR_ARCORE_PLANE_RENDER_H

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <android/asset_manager.h>
#include <cstdlib>

#include "arcore_c_api.h"
#include "util.h"

class ArcorePlaneRender {
public:
    ArcorePlaneRender() = default;
    ~ArcorePlaneRender() = default;

    // Sets up OpenGL state used by the plane renderer.  Must be called on the
    // OpenGL thread.
    void InitializeGlContent();

    // Draws the provided plane.
    void Draw(const glm::mat4& projection_mat, const glm::mat4& view_mat,
              const ArSession& ar_session, const ArPlane& ar_plane);

private:
    void UpdateForPlane(const ArSession& ar_session, const ArPlane& ar_plane);

    std::vector<glm::vec3> vertices_;
    std::vector<GLushort> triangles_;
    glm::mat4 model_mat_ = glm::mat4(1.0f);
    glm::vec3 normal_vec_ = glm::vec3(0.0f);

    GLuint texture_id_;

    GLuint shader_program_;
    GLint attri_vertices_;
    GLint uniform_mvp_mat_;
    GLint uniform_texture_;
    GLint uniform_model_mat_;
    GLint uniform_normal_vec_;
};


#endif //LARKXR_ARCORE_PLANE_RENDER_H
