//
// Created by 杨天正 on 2022/9/18.
//

#ifndef VIDEOPLAYER_MEDIAPLAYER_H
#define VIDEOPLAYER_MEDIAPLAYER_H

#include <jni.h>
#include "DeMuxer.h"
#include <android/native_window.h>

#define JAVA_PLAYER_EVENT_CALLBACK_API_NAME "playerEventCallback"


class MediaPlayer {

public:
    MediaPlayer(){};
    ~MediaPlayer(){};

    void Init(JNIEnv *jniEnv, jobject obj);
    void UnInit();

//    void setVideoPlayerRender(VideoRender *videoRender);

    void Play( char *url, ANativeWindow* window, int width, int height);

    void Pause();

    void Stop();

    void SeekToPosition(float position);

    long GetMediaParams(int paramType);

private:
    DeMuxer* muxer =  nullptr;

    static void PostMessage(void *context, int msgType, float msgCode);

    JNIEnv* GetJNIEnv(bool *isAttach);
    jobject GetJavaObj();
    JavaVM* GetJavaVM();
    JavaVM* m_JavaVM = nullptr;
    jobject m_JavaObj = nullptr;
};


#endif //VIDEOPLAYER_MEDIAPLAYER_H
