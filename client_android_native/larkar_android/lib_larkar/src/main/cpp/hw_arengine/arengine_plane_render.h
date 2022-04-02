//
// Created by fcx@pingxingyung.com on 2022/3/23.
// Copyright (c) 2022 www.pingxingyun.com All rights reserved.
//

#ifndef LARKXR_ARENGINE_PLANE_RENDER_H
#define LARKXR_ARENGINE_PLANE_RENDER_H

#include <vector>

#include <GLES2/gl2.h>
#include <util.h>

#include "huawei_arengine_interface.h"

class ArenginePlaneRender {
public:
    ArenginePlaneRender() = default;
    ~ArenginePlaneRender() = default;

    // Initialize the OpenGL state used by the plane renderer.
    void InitializePlaneGlContent();

    // Draw the provided plane.
    // @param projectionMat Draw the plane projection information matrix.
    // @param viewMat Draw the plane view information matrix.
    // @param session Query the sessions in the plane drawing.
    // @param plane Plane information of the real world in plane drawing.
    // @param color Color configuration of a plane.
    void
    Draw(const glm::mat4 &projectionMat, const glm::mat4 &viewMat, const HwArSession *session,
         const HwArPlane *plane, const glm::vec3 &color);

private:
    void UpdateForPlane(const HwArSession *session, const HwArPlane *plane);
    void DeleatPlane();

    std::vector<glm::vec3> vertices;
    std::vector<GLushort> triangles;
    glm::mat4 modelMat = glm::mat4(1.0f);
    //trigrid纹理
    GLuint textureId;

    GLuint mShaderProgram;
    GLint mAttriVertices;
    GLint mUniformMvpMat;
    GLint mUniformTexture;
    GLint mUniformModelMat;
    GLint mUniformNormalVec;
    GLint mUniformColor;

    glm::quat normalVec = glm::vec3(0.0f);
};


#endif //LARKXR_ARENGINE_PLANE_RENDER_H
