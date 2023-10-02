//
// Created by 杨天正 on 2022/9/10.
//

#include <jni.h>
#include <string>
#include <unistd.h>
#include "VideoPlayer.h"
#include "AudioPlayer.h"
#include <android/native_window.h>
#include <android/native_window_jni.h>

#include "media/decoder/base_decoder.h"
#include "media/decoder/v_decoder.h"
#include "media/render/video/video_render.h"
#include "media/render/video/native_render.h"
#include "media/player/player.h"
#include "media/render/audio/audio_render.h"
#include "media/render/audio/opensl_render.h"
#include "media/const.h"


extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavfilter/avfilter.h>
#include <libavcodec/jni.h>
}

#define JNI_FUNC(retType, bindClass, name)  JNIEXPORT retType JNICALL Java_com_example_ffmepglibrary_##bindClass##_##name
#define JNI_ARGS    JNIEnv *env, jobject thiz

VideoPlayer *videoPlayer;
AudioPlayer *audioPlayer;

VideoDecoder *videoDecoder;

extern "C" {
JNI_FUNC(jstring, FFmepgHelper, getFFmpegVersion)(JNI_ARGS) {
    const char *version = av_version_info();
    return env->NewStringUTF(version);
}

JNI_FUNC(jstring, FFmepgHelper, getFFmpegConfiguration)(JNI_ARGS) {
    const char *config = avcodec_configuration();
    return env->NewStringUTF(config);
}

JNI_FUNC(jint, VideoPlayer, init)(JNI_ARGS, jstring m_local_path, jobject surface, jint width,
                                  jint height) {
    const char *url = env->GetStringUTFChars(m_local_path, nullptr);
//    videoPlayer = new VideoPlayer();
//    ANativeWindow* window = ANativeWindow_fromSurface(env, surface);
//    int result = videoPlayer->init(url, window, width, height);
    audioPlayer = new AudioPlayer();
    int result = audioPlayer->init(url);
    return result;
}

JNI_FUNC(void, VideoPlayer, start)(JNI_ARGS) {
//    videoPlayer->start();
    audioPlayer->start();
}

JNI_FUNC(void, VideoPlayer, release)(JNI_ARGS) {
//    videoPlayer->release();
    audioPlayer->release();
}
}
extern "C"
JNIEXPORT jlong JNICALL
Java_com_example_videoplayer_FFmpegActivity_createPlayer(JNIEnv *env, jobject thiz, jstring path,
                                                         jobject surface) {
    auto player = new Player(env, path, surface);
    return (jlong) player;
}
extern "C"
JNIEXPORT void JNICALL
Java_com_example_videoplayer_FFmpegActivity_play(JNIEnv *env, jobject thiz, jlong player) {
    auto *p = (Player *) player;
    p->play();
}
extern "C"
JNIEXPORT void JNICALL
Java_com_example_videoplayer_FFmpegActivity_pause(JNIEnv *env, jobject thiz, jlong player) {
    auto *p = (Player *) player;
    p->pause();
}