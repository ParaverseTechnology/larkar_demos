//
// Created by fcx@pingxingyung.com on 2022/3/23.
// Copyright (c) 2022 www.pingxingyun.com All rights reserved.
//

#include <util.h>
#include "arengine_background_render.h"

namespace {
    // Positions of the quad vertices in the clip space (X, Y, Z).
    const GLfloat VERTICES[] = {
            -1.0f, -1.0f, 0.0f, +1.0f, -1.0f, 0.0f,
            -1.0f, +1.0f, 0.0f, +1.0f, +1.0f, 0.0f,
    };

    // UV coordinates of the quad vertices (S, T).
    const GLfloat UVS[] = {
            0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
    };

    constexpr char VERTEX_SHADER[] = R"(
        attribute vec4 vertex;
        attribute vec2 textureCoords;
        varying vec2 v_textureCoords;
        void main() {
            v_textureCoords = textureCoords;
            gl_Position = vertex;
        })";

    constexpr char FRAGMENT_SHADER[] = R"(
        #extension GL_OES_EGL_image_external : require
        precision mediump float;
        uniform samplerExternalOES texture;
        varying vec2 v_textureCoords;
        void main() {
            gl_FragColor = texture2D(texture, v_textureCoords);
        })";
}

void ArengineBackgroundRender::InitializeBackGroundGlContent() {
    shaderProgram = gWorldAr::util::CreateProgram(VERTEX_SHADER, FRAGMENT_SHADER);
    if (!shaderProgram) {
        LOGE("Could not create program.");
    }

    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_EXTERNAL_OES, textureId);
    glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    uniformTexture = glGetUniformLocation(shaderProgram, "texture");
    attributeVertices = glGetAttribLocation(shaderProgram, "vertex");
    attributeUvs = glGetAttribLocation(shaderProgram, "textureCoords");
}

void ArengineBackgroundRender::Draw(const HwArSession *session, const HwArFrame *frame) {
    // In OpenGLES, the texture coordinate dimension is 2.
    static_assert(std::extent<decltype(UVS)>::value == VERTICES_NUM * 2,
                  "Incorrect kUvs length");

    // The dimension of the vertex in OpenGLES is 3.
    static_assert(std::extent<decltype(VERTICES)>::value == VERTICES_NUM * 3,
                  "Incorrect kVertices length");

    // If the display rotation changes (including the view size change), you need to re-query the UVs.
    int32_t geometryChanged = 0;
    HwArFrame_getDisplayGeometryChanged(session, frame, &geometryChanged);

    // In OpenGLES, the texture coordinate dimension is 2.
    if (geometryChanged != 0 || !uvsInitialized) {
        // In OpenGLES, the texture coordinate dimension is 2.
        HwArFrame_transformDisplayUvCoords(session, frame, VERTICES_NUM * 2, UVS,
                                           transformedUvs);
        uvsInitialized = true;
    }

    glUseProgram(shaderProgram);
    glDepthMask(GL_FALSE);

    glUniform1i(uniformTexture, 1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_EXTERNAL_OES, textureId);

    // In OpenGLES, the dimension of the vertex is 3.
    glEnableVertexAttribArray(attributeVertices);
    glVertexAttribPointer(attributeVertices, 3, GL_FLOAT, GL_FALSE, 0,
                          VERTICES);

    // In OpenGLES, the texture coordinate dimension is 2.
    glEnableVertexAttribArray(attributeUvs);
    glVertexAttribPointer(attributeUvs, 2, GL_FLOAT, GL_FALSE, 0,
                          transformedUvs);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4); // Number of points.
    glUseProgram(0);
    glDepthMask(GL_TRUE);
    gWorldAr::util::CheckGlError("WorldBackgroundRenderer::Draw() error");
}

GLuint ArengineBackgroundRender::GetTextureId() const {
    return textureId;
}
