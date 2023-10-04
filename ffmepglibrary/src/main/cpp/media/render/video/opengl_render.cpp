//
// Created by yangpc on 2023/10/3.
//

#include <thread>
#include <unistd.h>
#include <libavutil/mem.h>
#include "opengl_render.h"

OpenglRender::OpenglRender(JNIEnv *env, DrawerProxy *drawerProxy) :m_drawer_proxy(drawerProxy){
    this->m_env = env;
    env->GetJavaVM(&m_jvm_for_thread);
    InitRenderThread();
}

OpenglRender::~OpenglRender() {
    delete m_egl_surface;
}

void OpenglRender::InitRenderThread() {
    std::shared_ptr<OpenglRender> that(this);
    std::thread t(sRenderThread, that);
    t.detach();
}

void OpenglRender::sRenderThread(std::shared_ptr<OpenglRender> that) {
    JNIEnv  *env;
    if (that->m_jvm_for_thread->AttachCurrentThread(&env, nullptr) != JNI_OK){
        LOGE(that->TAG, "线程初始化异常");
        return;
    }
    if (!that->InitEGL()){
        that->m_jvm_for_thread->DetachCurrentThread();
        return;
    }

    while (true){
        switch (that->m_state) {
            case FRESH_SURFACE:
                LOGI(that->TAG, "Loop Render FRESH_SURFACE");
                that->InitDspWindow(env);
                that->CreateSurface();
                that->m_state = RENDERING;
                break;
            case RENDERING:
                that->Render();
                break;
            case SURFACE_DESTROY:
                LOGI(that->TAG, "Loop Render SURFACE_DESTROY");
                that->DestroySurface();
                that->m_state = NO_SURFACE;
                break;
            case STOP:
                LOGI(that->TAG, "Loop Render STOP") ;
                //解除线程和jvm关联
                that->ReleaseRender();
                that->m_jvm_for_thread->DetachCurrentThread();
                return;
            case NO_SURFACE:
            default:
                break;
        }
        usleep(20 * 1000);
    }
}

bool OpenglRender::InitEGL() {
    m_egl_surface = new EglSurface();
    return m_egl_surface->Init();
}
void OpenglRender::SetSurface(jobject surface) {
    if (surface != nullptr){
        m_surface_ref = m_env->NewGlobalRef(surface);
        m_state = FRESH_SURFACE;
    } else{
        m_env->DeleteGlobalRef(m_surface_ref);
        m_state = SURFACE_DESTROY;
    }
}

void OpenglRender::InitDspWindow(JNIEnv *env) {
    if (m_surface_ref != nullptr){
        m_native_window = ANativeWindow_fromSurface(env,m_surface_ref);

        m_window_width = ANativeWindow_getWidth(m_native_window);
        m_window_height = ANativeWindow_getHeight(m_native_window);

        ANativeWindow_setBuffersGeometry(m_native_window,
                                         m_window_width, m_window_height,  WINDOW_FORMAT_RGBA_8888);
        LOGD(TAG, "View Port width: %d, height: %d", m_window_width, m_window_height)
    }
}

void OpenglRender::CreateSurface() {
    m_egl_surface->CreateEglSurface(m_native_window,m_window_width, m_window_height);
    glViewport(0, 0, m_window_width, m_window_height);
}

void OpenglRender::Render() {
    if (RENDERING == m_state){
        m_drawer_proxy->Draw();
        m_egl_surface->SwapBuffers();
    }
}

void OpenglRender::Stop() {
    m_state = STOP;
}

void OpenglRender::ReleaseRender() {
    ReleaseDrawers();
    ReleaseSurface();
    ReleaseWindow();
}

void OpenglRender::ReleaseDrawers() {
    if (m_drawer_proxy != nullptr){
        m_drawer_proxy->Release();
        delete m_drawer_proxy;
        m_drawer_proxy = nullptr;
    }
}

void OpenglRender::ReleaseSurface() {
    if (m_egl_surface != nullptr){
        m_egl_surface->Release();
        delete m_egl_surface;
        m_egl_surface = nullptr;
    }
}

void OpenglRender::ReleaseWindow() {
    if (m_native_window != nullptr){
        ANativeWindow_release(m_native_window);
        m_native_window = nullptr;
    }
}

void OpenglRender::SetOffScreenSize(int width, int height) {
    m_window_width = width;
    m_window_height = height;
    m_state = FRESH_SURFACE;
}

void OpenglRender::DestroySurface() {
    m_egl_surface->DestroyEglSurface();
    ReleaseWindow();
}

