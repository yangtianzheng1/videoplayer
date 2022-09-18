//
// Created by 杨天正 on 2022/9/18.
//

#include "MediaPlayer.h"

void MediaPlayer::Init(JNIEnv* jniEnv, jobject obj) {
    jniEnv->GetJavaVM(&m_JavaVM);
    m_JavaObj = jniEnv->NewGlobalRef(obj);
    av_jni_set_java_vm(m_JavaVM, nullptr);
    muxer = new DeMuxer();
    muxer->init(SOFT);
    muxer->setMessageCallback(this, PostMessage);
}

//void MediaPlayer::setVideoPlayerRender(VideoRender *videoRender) {
//    muxer->setVideoRender(videoRender);
//}

void MediaPlayer::UnInit() {
    muxer->unInit();
    delete muxer;
    muxer = nullptr;

    bool isAttach = false;
    GetJNIEnv(&isAttach)->DeleteGlobalRef(m_JavaObj);
    if (isAttach)
        GetJavaVM()->DetachCurrentThread();
}

void MediaPlayer::Play( char *url, ANativeWindow* window, int width, int height) {
    muxer->start(url, window, width, height);
}

void MediaPlayer::Pause() {
    muxer->pause();
}

void MediaPlayer::Stop() {
    muxer->stop();
}

void MediaPlayer::SeekToPosition(float position) {
    muxer->seekToPosition(position);
}

long MediaPlayer::GetMediaParams(int paramType) {
    long value = 0;
//    switch (paramType) {
//        case MEDIA_PARAM_VIDEO_WIDTH:
//            value = muxer != nullptr ? muxer->GetVideoWidth() : 0;
//            break;
//        case MEDIA_PARAM_VIDEO_HEIGHT:
//            value = m_VideoDecoder != nullptr ? m_VideoDecoder->GetVideoHeight() : 0;
//            break;
//        case MEDIA_PARAM_VIDEO_DURATION:
//            value = m_VideoDecoder != nullptr ? m_VideoDecoder->GetDuration() : 0;
//            break;
//    }
    return value;
}

JNIEnv *MediaPlayer::GetJNIEnv(bool *isAttach) {
    JNIEnv *env;
    int status;
    if (nullptr == m_JavaVM) {
        return nullptr;
    }
    *isAttach = false;
    status = m_JavaVM->GetEnv((void **) &env, JNI_VERSION_1_4);
    if (status != JNI_OK) {
        status = m_JavaVM->AttachCurrentThread(&env, nullptr);
        if (status != JNI_OK) {
            return nullptr;
        }
        *isAttach = true;
    }
    return env;
}

jobject MediaPlayer::GetJavaObj() {
    return m_JavaObj;
}

JavaVM *MediaPlayer::GetJavaVM() {
    return m_JavaVM;
}

void MediaPlayer::PostMessage(void *context, int msgType, float msgCode) {
    if (context != nullptr) {
        MediaPlayer *player = static_cast<MediaPlayer *>(context);
        bool isAttach = false;
        JNIEnv *env = player->GetJNIEnv(&isAttach);
        if (env == nullptr)
            return;
        jobject javaObj = player->GetJavaObj();
        jmethodID mid = env->GetMethodID(env->GetObjectClass(javaObj),
                                         JAVA_PLAYER_EVENT_CALLBACK_API_NAME, "(IF)V");
        env->CallVoidMethod(javaObj, mid, msgType, msgCode);
        if (isAttach)
            player->GetJavaVM()->DetachCurrentThread();
    }
}

