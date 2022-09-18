//
// Created by 杨天正 on 2022/9/10.
//

#include <jni.h>
#include <string>
#include <unistd.h>
#include "MediaPlayer.h"
#include <android/native_window.h>
#include <android/native_window_jni.h>


extern "C" {
#include "include/libavcodec/avcodec.h"
#include "include/libavformat/avformat.h"
#include "include/libavcodec/jni.h"
}

#define JNI_FUNC(retType, bindClass, name)  JNIEXPORT retType JNICALL Java_com_example_ffmepglibrary_##bindClass##_##name
#define JNI_ARGS    JNIEnv *env, jobject thiz

extern "C" {
JNI_FUNC(jstring, FFmepgHelper, getFFmpegVersion)(JNI_ARGS) {
    const char *version = av_version_info();
    return env->NewStringUTF(version);
}

JNI_FUNC(jstring, FFmepgHelper, getFFmpegConfiguration)(JNI_ARGS) {
    const char *config = avcodec_configuration();
    return env->NewStringUTF(config);
}

JNI_FUNC(jlong, VideoPlayer, nativeCreateMediaPlayEngine)(JNI_ARGS){
    auto *ffMediaPlayer = new MediaPlayer();
    return reinterpret_cast<jlong>(ffMediaPlayer);
}

JNI_FUNC(void, VideoPlayer, nativeDestroy)(JNI_ARGS, jlong native_engine_ctr){
    auto *ffMediaPlayer = reinterpret_cast<MediaPlayer *>(native_engine_ctr);
    ffMediaPlayer->UnInit();
}

JNI_FUNC(void, VideoPlayer, nativeInit)(JNI_ARGS, jlong native_engine_ctr){
    auto *ffMediaPlayer = reinterpret_cast<MediaPlayer *>(native_engine_ctr);
    ffMediaPlayer->Init(env, thiz);
}

JNI_FUNC(void, VideoPlayer, nativePlay)(JNI_ARGS, jlong native_engine_ctr,
        jstring m_local_path, jobject surface, jint width,
                                  jint height) {
    const char *url = env->GetStringUTFChars(m_local_path, nullptr);
    auto *ffMediaPlayer = reinterpret_cast<MediaPlayer *>(native_engine_ctr);
    ANativeWindow* window = ANativeWindow_fromSurface(env, surface);
    ffMediaPlayer->Play(const_cast<char *>(url), window, width, height);
}

JNI_FUNC(void, VideoPlayer, nativePause)(JNI_ARGS,jlong native_engine_ctr) {
    auto *ffMediaPlayer = reinterpret_cast<MediaPlayer *>(native_engine_ctr);
    ffMediaPlayer->Pause();
}

JNI_FUNC(void, VideoPlayer, nativeStop)(JNI_ARGS,jlong native_engine_ctr) {
    auto *ffMediaPlayer = reinterpret_cast<MediaPlayer *>(native_engine_ctr);
    ffMediaPlayer->Stop();
}

JNI_FUNC(void, VideoPlayer, nativeSeekToPosition)(JNI_ARGS,jlong native_engine_ctr, jfloat position) {
    auto *ffMediaPlayer = reinterpret_cast<MediaPlayer *>(native_engine_ctr);
    ffMediaPlayer->SeekToPosition(position);
}

JNI_FUNC(void, VideoPlayer, nativeGetMediaParams)(JNI_ARGS,jlong native_engine_ctr, jint param_type) {
    auto *ffMediaPlayer = reinterpret_cast<MediaPlayer *>(native_engine_ctr);
    ffMediaPlayer->GetMediaParams(param_type);
}
}