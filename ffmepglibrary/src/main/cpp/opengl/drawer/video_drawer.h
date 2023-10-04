//
// Created by yangpc on 2023/10/4.
//

#ifndef VIDEOPLAYER_VIDEO_DRAWER_H
#define VIDEOPLAYER_VIDEO_DRAWER_H


#include "../../media/render/video/video_render.h"
#include "../egl/egl_surface.h"
#include "drawer.h"

class VideoDrawer: public Drawer, public VideoRender {

public:
    VideoDrawer();
    ~VideoDrawer();

    void InitRender(JNIEnv *env, int video_width, int video_height, int *dst_size) override ;
    void Render(OneFrame *one_frame) override ;
    void ReleaseRender() override ;


    const char* GetVertexShader() override;
    const char* GetFragmentShader() override;
    void InitCstShaderHandler() override;
    void BindTexture() override;
    void PrepareDraw() override;
    void DoneDraw() override;
};


#endif //VIDEOPLAYER_VIDEO_DRAWER_H
