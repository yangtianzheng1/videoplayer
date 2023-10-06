//
// Created by yangpc on 2023/10/3.
//

#ifndef VIDEOPLAYER_OPENGL_RENDER_H
#define VIDEOPLAYER_OPENGL_RENDER_H

#include <android/native_window.h>
#include <android/native_window_jni.h>
#include <jni.h>
#include "video_render.h"
#include "../../../opengl/drawer/proxy/drawer_proxy.h"
#include "../../../opengl/egl/egl_surface.h"
#include "opengl_pixel_receiver.h"
#include <memory>

class OpenglRender {

private:
    const char* TAG = "OpenGlRender";

    enum STATE{
        NO_SURFACE,
        FRESH_SURFACE,
        RENDERING,
        SURFACE_DESTROY,
        STOP
    };

    JNIEnv *m_env = nullptr;

    JavaVM *m_jvm_for_thread = nullptr;

    jobject m_surface_ref = nullptr;

    ANativeWindow *m_native_window = nullptr;

    EglSurface *m_egl_surface = nullptr;

    DrawerProxy *m_drawer_proxy = nullptr;

    int m_window_width = 0;
    int m_window_height = 0;

    bool m_need_output_pixels = false;

    OpenGLPixelReceiver * m_pixel_receiver = NULL;

    STATE m_state = NO_SURFACE;

    void InitRenderThread();
    bool InitEGL();
    void InitDspWindow(JNIEnv *env);

    void CreateSurface();
    void DestroySurface();

    void Render();

    void ReleaseRender();
    void ReleaseDrawers();
    void ReleaseSurface();
    void ReleaseWindow();

    static void sRenderThread(std::shared_ptr<OpenglRender> that);


public:
    OpenglRender(JNIEnv* env, DrawerProxy* drawerProxy);
    ~OpenglRender();

    void SetPixelReceiver(OpenGLPixelReceiver *receiver) {
        m_pixel_receiver = receiver;
    }

    void SetSurface(jobject surface);
    void SetOffScreenSize(int width, int height);
    void RequestRgbaData();
    void Stop();
};


#endif //VIDEOPLAYER_OPENGL_RENDER_H
