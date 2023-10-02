//
// Created by yangpc on 2023/10/1.
//

#ifndef VIDEOPLAYER_VIDEO_RENDER_H
#define VIDEOPLAYER_VIDEO_RENDER_H

#include <jni.h>
#include <stdint.h>
#include "../../one_frame.h"

class VideoRender{

public:
    virtual void InitRender(JNIEnv *env, int video_width, int video_height, int *dst_size) = 0;
    virtual void ReleaseRender() = 0;
    virtual void Render(OneFrame *one_frame) = 0;

};

#endif //VIDEOPLAYER_VIDEO_RENDER_H
