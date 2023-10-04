//
// Created by yangpc on 2023/10/2.
//

#ifndef VIDEOPLAYER_EGL_CORE_H
#define VIDEOPLAYER_EGL_CORE_H

extern "C" {
#include <EGL/egl.h>
#include <EGL/eglext.h>
};

class EglCore {

private:
    const char *TAG = "EglCore";
    EGLDisplay m_egl_dsp = EGL_NO_DISPLAY;
    EGLContext m_egl_cxt = EGL_NO_CONTEXT;

    EGLConfig m_egl_cfg{};

    EGLConfig GetEGLConfig();

public:

    EglCore();

    ~EglCore();

    bool Init(EGLContext share_ctx);

    EGLSurface CreateWindSurface(ANativeWindow *window);

    EGLSurface CreateOffScreenSurface(int width, int height);

    void MakeCurrent(EGLSurface eglSurface);

    void SwapBuffers(EGLSurface eglSurface);

    void DestroySurface(EGLSurface eglSurface);

    void Release();
};

#endif //VIDEOPLAYER_EGL_CORE_H
