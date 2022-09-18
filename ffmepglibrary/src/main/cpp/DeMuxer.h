//
// Created by 杨天正 on 2022/9/16.
//

#ifndef VIDEOPLAYER_DEMUXER_H
#define VIDEOPLAYER_DEMUXER_H

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/frame.h>
#include <libavutil/time.h>
#include <libavcodec/jni.h>
};

#include <thread>
#include "VideoPlayer.h"
#include "MessageCallback.h"
#include "AudioPlayer.h"
#include <android/native_window.h>

using namespace std;

class DeMuxer {

private:
    //封装格式上下文
    AVFormatContext* m_AVFormatContext = nullptr;
    thread* decoderThread = nullptr;
    VideoPlayer* m_VideoPlayer = nullptr;
    AudioPlayer* m_AudioPlayer = nullptr;

    //解码器状态
    volatile DecoderState  m_DecoderState = STATE_UNKNOWN;
    mutex               m_Mutex;
    condition_variable  m_Cond;
    //文件地址
    char m_Url[2048] = {0};
    volatile float m_SeekPosition = 0;
    volatile bool m_SeekSuccess = false;
    //总时长 ms
    long m_Duration = 0;
    static void doAVDecoding(DeMuxer *deMuxer);
    void decodingLoop();
    int decodeOnePacket();

public:
    void start(const char *url, ANativeWindow* window, int width, int height);
    void pause();
    void resume();
    void stop();

    void init(DecoderType decoderType );

    void unInit();

    void seekToPosition(float position);

    long getCurrentPosition();

    void setMessageCallback(void *context, MessageCallback callback);
};


#endif //VIDEOPLAYER_DEMUXER_H
