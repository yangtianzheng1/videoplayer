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

//#include "media/decoder/base_decoder.h"
//#include "media/decoder/v_decoder.h"
//#include "media/render/video/video_render.h"
//#include "media/render/video/native_render.h"
#include "media/player/player.h"
//#include "media/render/audio/audio_render.h"
//#include "media/render/audio/opensl_render.h"
//#include "media/const.h"
#include "media/player/gl_player.h"
#include "media/muxer/ff_repack.h"
#include "media/synthesizer/synthesizer.h"


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
extern "C"
JNIEXPORT jlong JNICALL
Java_com_example_videoplayer_FFmpegGLPlayerActivity_createGLPlayer(JNIEnv *env, jobject thiz,
                                                                   jstring path, jobject surface) {
    GlPlayer *player = new GlPlayer(env, path);
    player->SetSurface(surface);
    return (jlong) player;
}
extern "C"
JNIEXPORT void JNICALL
Java_com_example_videoplayer_FFmpegGLPlayerActivity_playOrPause(JNIEnv *env, jobject thiz,
                                                                jlong player) {
    GlPlayer *p = (GlPlayer *) player;
    p->PlayOrPause();
}
extern "C"
JNIEXPORT void JNICALL
Java_com_example_videoplayer_FFmpegGLPlayerActivity_stop(JNIEnv *env, jobject thiz, jlong player) {
        GlPlayer *p = (GlPlayer *) player;
        p->Release();
}
extern "C"
JNIEXPORT jlong JNICALL
Java_com_example_videoplayer_FFRepackActivity_createRepack(JNIEnv *env, jobject thiz,
                                                           jstring src_path, jstring dest_path) {
    FFRepack *repack = new FFRepack(env, src_path, dest_path);
    return (jlong) repack;
}
extern "C"
JNIEXPORT void JNICALL
Java_com_example_videoplayer_FFRepackActivity_startRepack(JNIEnv *env, jobject thiz, jlong repack) {
    FFRepack *ffRepack = (FFRepack *) repack;
    ffRepack->Start();
}
extern "C"
JNIEXPORT jlong JNICALL
Java_com_example_videoplayer_FFEncodeActivity_initEncoder(JNIEnv *env, jobject thiz,
                                                          jstring src_path, jstring dest_path) {
    Synthesizer *synthesizer = new Synthesizer(env, src_path, dest_path);
    return (jlong)synthesizer;
}
extern "C"
JNIEXPORT void JNICALL
Java_com_example_videoplayer_FFEncodeActivity_startEncoder(JNIEnv *env, jobject thiz,
                                                           jlong encoder) {
    Synthesizer *s =  (Synthesizer *)encoder;
    s->Start();
}
extern "C"
JNIEXPORT void JNICALL
Java_com_example_videoplayer_FFEncodeActivity_releaseEncoder(JNIEnv *env, jobject thiz,
                                                             jlong encoder) {
    Synthesizer *s =  (Synthesizer *)encoder;
    delete s;
}