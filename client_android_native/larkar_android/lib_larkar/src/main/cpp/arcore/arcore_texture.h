//
// Created by fcx@pingxingyung.com on 2022/3/24.
// Copyright (c) 2022 www.pingxingyun.com All rights reserved.
//

#ifndef LARKXR_ARCORE_TEXTURE_H
#define LARKXR_ARCORE_TEXTURE_H

#include "arcore_c_api.h"

class ArcoreTexture {
public:
    ArcoreTexture() = default;
    ~ArcoreTexture() = default;

    void CreateOnGlThread();
    void UpdateWithDepthImageOnGlThread(const ArSession& session,
                                        const ArFrame& frame);
    unsigned int GetTextureId() { return texture_id_; }

    unsigned int GetWidth() { return width_; }

    unsigned int GetHeight() { return height_; }

private:
    unsigned int texture_id_ = 0;
    unsigned int width_ = 1;
    unsigned int height_ = 1;
};


#endif //LARKXR_ARCORE_TEXTURE_H
