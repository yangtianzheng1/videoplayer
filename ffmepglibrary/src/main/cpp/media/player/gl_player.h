//
// Created by yangpc on 2023/10/4.
//

#ifndef VIDEOPLAYER_GL_PLAYER_H
#define VIDEOPLAYER_GL_PLAYER_H


#include "../decoder/v_decoder.h"
#include "../../opengl/drawer/proxy/drawer_proxy.h"
#include "../../opengl/drawer/video_drawer.h"
#include "../decoder/a_decoder.h"
#include "../render/video/opengl_render.h"

class GlPlayer {

private:
    VideoDecoder *m_v_decoder;
    OpenglRender *m_gl_render;

    DrawerProxy *m_v_drawer_proxy;
    VideoDrawer *m_v_drawer;

    AudioDecoder *m_a_decoder;
    AudioRender *m_a_render;

public:
    GlPlayer(JNIEnv *jniEnv, jstring path);
    ~GlPlayer();

    void SetSurface(jobject surface);
    void PlayOrPause();
    void Release();
};


#endif //VIDEOPLAYER_GL_PLAYER_H
