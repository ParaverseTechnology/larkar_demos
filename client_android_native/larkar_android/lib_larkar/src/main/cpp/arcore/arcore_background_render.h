//
// Created by fcx@pingxingyung.com on 2022/3/24.
// Copyright (c) 2022 www.pingxingyun.com All rights reserved.
//

#ifndef LARKXR_ARCORE_BACKGROUND_RENDER_H
#define LARKXR_ARCORE_BACKGROUND_RENDER_H

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <android/asset_manager.h>
#include <cstdlib>

#include "arcore_c_api.h"
#include "util.h"

class ArcoreBackgroundRender {
public:
    ArcoreBackgroundRender() = default;
    ~ArcoreBackgroundRender() = default;

    // Sets up OpenGL state.  Must be called on the OpenGL thread and before any
    // other methods below.
    void InitializeGlContent(int depthTextureId);

    // Draws the background image.  This methods must be called for every ArFrame
    // returned by ArSession_update() to catch display geometry change events.
    //  debugShowDepthMap Toggles whether to show the live camera feed or latest
    //  depth image.
    void Draw(const ArSession* session, const ArFrame* frame,
              bool debug_show_depth_map);

    // Returns the generated texture name for the GL_TEXTURE_EXTERNAL_OES target.
    GLuint GetTextureId() const;
private:
    static constexpr int kNumVertices = 4;

    GLuint camera_program_;
    GLuint depth_program_;

    GLuint camera_texture_id_;
    GLuint depth_texture_id_;

    GLuint camera_position_attrib_;
    GLuint camera_tex_coord_attrib_;
    GLuint camera_texture_uniform_;

    GLuint depth_texture_uniform_;
    GLuint depth_position_attrib_;
    GLuint depth_tex_coord_attrib_;

    float transformed_uvs_[kNumVertices * 2];
    bool uvs_initialized_ = false;
};


#endif //LARKXR_ARCORE_BACKGROUND_RENDER_H
