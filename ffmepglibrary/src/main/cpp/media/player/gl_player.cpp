//
// Created by yangpc on 2023/10/4.
//

#include "gl_player.h"
#include "../../opengl/drawer/proxy/def_drawer_proxy_impl.h"
#include "../render/audio/opensl_render.h"

GlPlayer::GlPlayer(JNIEnv *jniEnv, jstring path) {
    m_v_decoder = new VideoDecoder(jniEnv, path);

    // OpenGL 渲染
    m_v_drawer = new VideoDrawer();
    m_v_decoder->SetRender(m_v_drawer);

    DefDrawerProxyImpl *proxyImpl =  new DefDrawerProxyImpl();
    proxyImpl->AddDrawer(m_v_drawer);

    m_v_drawer_proxy = proxyImpl;

    m_gl_render = new OpenglRender(jniEnv, m_v_drawer_proxy);

    // 音频解码
    m_a_decoder = new AudioDecoder(jniEnv, path, false);
    m_a_render = new OpenSLRender1();
    m_a_decoder->SetRender(m_a_render);
}

GlPlayer::~GlPlayer() {
    // 此处不需要 delete 成员指针
    // 在BaseDecoder 和 OpenGLRender 中的线程已经使用智能指针，会自动释放相关指针
}

void GlPlayer::SetSurface(jobject surface) {
    m_gl_render->SetSurface(surface);
}

void GlPlayer::PlayOrPause() {
    if (!m_v_decoder->IsRunning()) {
        LOGI("Player", "播放视频")
        m_v_decoder->GoOn();
    } else {
        LOGI("Player", "暂停视频")
        m_v_decoder->Pause();
    }
    if (!m_a_decoder->IsRunning()) {
        LOGI("Player", "播放音频")
        m_a_decoder->GoOn();
    } else {
        LOGI("Player", "暂停音频")
        m_a_decoder->Pause();
    }
}

void GlPlayer::Release() {
    m_gl_render->Stop();
    m_v_decoder->Stop();
    m_a_decoder->Stop();
}