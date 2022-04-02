//
// Created by fcx@pingxingyung.com on 2022/3/24.
// Copyright (c) 2022 www.pingxingyun.com All rights reserved.
//

#include "arcore_background_render.h"

namespace {
// Positions of the quad vertices in clip space (X, Y).
    const GLfloat kVertices[] = {
            -1.0f, -1.0f, +1.0f, -1.0f, -1.0f, +1.0f, +1.0f, +1.0f,
    };

    constexpr char kCameraVertexShader[] = R"(
        attribute vec4 a_Position;
        attribute vec2 a_TexCoord;

        varying vec2 v_TexCoord;

        void main() {
           gl_Position = a_Position;
           v_TexCoord = a_TexCoord;
        })";

    constexpr char kCameraFragmentShader[] = R"(
            #extension GL_OES_EGL_image_external : require
            precision mediump float;
            varying vec2 v_TexCoord;
            uniform samplerExternalOES sTexture;
            void main() {
                gl_FragColor = texture2D(sTexture, v_TexCoord);
            })";

    constexpr char kDepthVisualizerVertexShader[] = R"(
        attribute vec4 a_Position;
        attribute vec2 a_TexCoord;

        varying vec2 v_TexCoord;

        void main() {
           v_TexCoord = a_TexCoord;
           gl_Position = a_Position;
        })";

    constexpr char kDepthVisualizerFragmentShader[] = R"(
        precision mediump float;

        uniform sampler2D u_DepthTexture;

        varying vec2 v_TexCoord;

        const highp float kMaxDepth = 8000.0; // In millimeters.

        float DepthGetMillimeters(in sampler2D depth_texture, in vec2 depth_uv) {
          // Depth is packed into the red and green components of its texture.
          // The texture is a normalized format, storing millimeters.
          vec3 packedDepthAndVisibility = texture2D(depth_texture, depth_uv).xyz;
          return dot(packedDepthAndVisibility.xy, vec2(255.0, 256.0 * 255.0));
        }

        // Returns a color corresponding to the depth passed in. Colors range from red
        // to green to blue, where red is closest and blue is farthest.
        //
        // Uses Turbo color mapping:
        // https://ai.googleblog.com/2019/08/turbo-improved-rainbow-colormap-for.html
        vec3 DepthGetColorVisualization(in float x) {
          const vec4 kRedVec4 = vec4(0.55305649, 3.00913185, -5.46192616, -11.11819092);
          const vec4 kGreenVec4 = vec4(0.16207513, 0.17712472, 15.24091500, -36.50657960);
          const vec4 kBlueVec4 = vec4(-0.05195877, 5.18000081, -30.94853351, 81.96403246);
          const vec2 kRedVec2 = vec2(27.81927491, -14.87899417);
          const vec2 kGreenVec2 = vec2(25.95549545, -5.02738237);
          const vec2 kBlueVec2 = vec2(-86.53476570, 30.23299484);
          const float kInvalidDepthThreshold = 0.01;

          // Adjusts color space via 6 degree poly interpolation to avoid pure red.
          x = clamp(x * 0.9 + 0.03, 0.0, 1.0);
          vec4 v4 = vec4(1.0, x, x * x, x * x * x);
          vec2 v2 = v4.zw * v4.z;
          vec3 polynomial_color = vec3(
            dot(v4, kRedVec4) + dot(v2, kRedVec2),
            dot(v4, kGreenVec4) + dot(v2, kGreenVec2),
            dot(v4, kBlueVec4) + dot(v2, kBlueVec2)
          );

          return step(kInvalidDepthThreshold, x) * polynomial_color;
        }

        void main() {
          highp float normalized_depth =
              clamp(DepthGetMillimeters(u_DepthTexture, v_TexCoord.xy) / kMaxDepth,
                    0.0, 1.0);
          vec4 depth_color = vec4(DepthGetColorVisualization(normalized_depth), 1.0);
          gl_FragColor = depth_color;
        })";

}

void ArcoreBackgroundRender::InitializeGlContent(int depth_texture_id) {
    glGenTextures(1, &camera_texture_id_);
    glBindTexture(GL_TEXTURE_EXTERNAL_OES, camera_texture_id_);
    glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    camera_program_ =
            gWorldAr::util::CreateProgram(kCameraVertexShader, kCameraFragmentShader);
    if (!camera_program_) {
        LOGE("Could not create program.");
    }

    camera_texture_uniform_ = glGetUniformLocation(camera_program_, "sTexture");
    camera_position_attrib_ = glGetAttribLocation(camera_program_, "a_Position");
    camera_tex_coord_attrib_ = glGetAttribLocation(camera_program_, "a_TexCoord");

    depth_program_ = gWorldAr::util::CreateProgram(kDepthVisualizerVertexShader, kDepthVisualizerFragmentShader);
    if (!depth_program_) {
        LOGE("Could not create program.");
    }

    depth_texture_uniform_ =
            glGetUniformLocation(depth_program_, "u_DepthTexture");
    depth_position_attrib_ = glGetAttribLocation(depth_program_, "a_Position");
    depth_tex_coord_attrib_ = glGetAttribLocation(depth_program_, "a_TexCoord");

    depth_texture_id_ = depth_texture_id;
}

void ArcoreBackgroundRender::Draw(const ArSession *session, const ArFrame *frame,
                                  bool debug_show_depth_map) {
    static_assert(std::extent<decltype(kVertices)>::value == kNumVertices * 2,
                  "Incorrect kVertices length");

    // If display rotation changed (also includes view size change), we need to
    // re-query the uv coordinates for the on-screen portion of the camera image.
    int32_t geometry_changed = 0;
    ArFrame_getDisplayGeometryChanged(session, frame, &geometry_changed);
    if (geometry_changed != 0 || !uvs_initialized_) {
        ArFrame_transformCoordinates2d(
                session, frame, AR_COORDINATES_2D_OPENGL_NORMALIZED_DEVICE_COORDINATES,
                kNumVertices, kVertices, AR_COORDINATES_2D_TEXTURE_NORMALIZED,
                transformed_uvs_);
        uvs_initialized_ = true;
    }

    int64_t frame_timestamp;
    ArFrame_getTimestamp(session, frame, &frame_timestamp);
    if (frame_timestamp == 0) {
        // Suppress rendering if the camera did not produce the first frame yet.
        // This is to avoid drawing possible leftover data from previous sessions if
        // the texture is reused.
        return;
    }

    if (depth_texture_id_ == -1 || camera_texture_id_ == -1) {
        return;
    }

    glDepthMask(GL_FALSE);

    if (debug_show_depth_map) {
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, depth_texture_id_);
        glUseProgram(depth_program_);
        glUniform1i(depth_texture_uniform_, 0);
        glUniform1i(camera_texture_uniform_, 1);

        // Set the vertex positions and texture coordinates.
        glVertexAttribPointer(depth_position_attrib_, 2, GL_FLOAT, false, 0,
                              kVertices);
        glVertexAttribPointer(depth_tex_coord_attrib_, 2, GL_FLOAT, false, 0,
                              transformed_uvs_);
        glEnableVertexAttribArray(depth_position_attrib_);
        glEnableVertexAttribArray(depth_tex_coord_attrib_);
    } else {
        glBindTexture(GL_TEXTURE_EXTERNAL_OES, camera_texture_id_);
        glUseProgram(camera_program_);
        glUniform1i(camera_texture_uniform_, 0);

        // Set the vertex positions and texture coordinates.
        glVertexAttribPointer(camera_position_attrib_, 2, GL_FLOAT, false, 0,
                              kVertices);
        glVertexAttribPointer(camera_tex_coord_attrib_, 2, GL_FLOAT, false, 0,
                              transformed_uvs_);
        glEnableVertexAttribArray(camera_position_attrib_);
        glEnableVertexAttribArray(camera_tex_coord_attrib_);
    }

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    // Disable vertex arrays
    if (debug_show_depth_map) {
        glDisableVertexAttribArray(depth_position_attrib_);
        glDisableVertexAttribArray(depth_tex_coord_attrib_);
    } else {
        glDisableVertexAttribArray(camera_position_attrib_);
        glDisableVertexAttribArray(camera_tex_coord_attrib_);
    }

    glUseProgram(0);
    glDepthMask(GL_TRUE);
    gWorldAr::util::CheckGlError("BackgroundRenderer::Draw() error");
}

GLuint ArcoreBackgroundRender::GetTextureId() const {
    return camera_texture_id_;
}
