//
// Created by yangpc on 2023/10/1.
//

#ifndef VIDEOPLAYER_NATIVE_RENDER_H
#define VIDEOPLAYER_NATIVE_RENDER_H

#include <android/native_window.h>
#include <android/native_window_jni.h>
#include <jni.h>

#include "video_render.h"
#include "../../one_frame.h"

extern "C"{
#include <libavutil/mem.h>
};

class NativeRender : public VideoRender{

private:
    const char  *TAG = "NativeRender";

    jobject m_surface_ref = nullptr;

    // 存放输出到屏幕的缓存数据
    ANativeWindow_Buffer m_out_buffer;

    // 本地窗口
    ANativeWindow *m_native_window = nullptr;

    //显示的目标宽
    int m_dst_w;

    //显示的目标高
    int m_dst_h;

public:
    NativeRender(JNIEnv *env, jobject surface);
    ~NativeRender();

    void InitRender(JNIEnv *env, int video_width, int video_height, int *dst_size) override ;
    void ReleaseRender() override ;
    void Render(OneFrame *one_frame) override ;
};


#endif //VIDEOPLAYER_NATIVE_RENDER_H
