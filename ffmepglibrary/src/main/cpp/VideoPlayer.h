//
// Created by 杨天正 on 2022/9/11.
//

#ifndef VIDEOPLAYER_VIDEOPLAYER_H
#define VIDEOPLAYER_VIDEOPLAYER_H

#include <android/native_window.h>
#include "AVPacketQueue.h"
#include "MessageCallback.h"

extern "C"{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
#include <libavutil/time.h>
}


class VideoPlayer {

public:
    VideoPlayer(){};
    ~VideoPlayer(){};
    void init();
    void unInit();
    void start(AVFormatContext *m_AVFormatContext, ANativeWindow *window, int width, int height);
    void UpdateTimeStamp();
    void AVSync();
    void pause();
    void resume();
    void stop();
    void clearCache();
    void setStreamIdx(int streamIdx) {
        m_StreamIdx = streamIdx;
    }

    int getStreamIdx(){
        return m_StreamIdx;
    }

    void pushAVPacket(AVPacket *avPacket) {
        m_PacketQueue->PushPacket(avPacket);
    }

    int getBufferSize() {
        return m_PacketQueue->GetSize();
    }

    void setMessageCallback(void *context, MessageCallback callback) {
        m_MsgContext = context;
        m_MsgCallback = callback;
    }

    long getCurrentPosition() {
        return m_CurTimeStamp;
    }

    //设置音视频同步的回调
    virtual void SetAVSyncCallback(void *context, AVSyncCallback callback) {
        m_AVDecoderContext = context;
        m_AVSyncCallback = callback;
    }

    static long long GetSysCurrentTime()
    {
        struct timeval time;
        gettimeofday(&time, NULL);
        long long curTime = ((long long)(time.tv_sec))*1000+time.tv_usec/1000;
        return curTime;
    }



protected:
    int m_StreamIdx = -1;
    AVPacketQueue *m_PacketQueue = nullptr;
    void *m_MsgContext = nullptr;
    MessageCallback m_MsgCallback = nullptr;
    //当前播放时间
    long m_CurTimeStamp = 0;
    long m_StartTimeStamp = 0;
    void *m_AVDecoderContext = nullptr;
    AVSyncCallback m_AVSyncCallback = nullptr;//用作音视频同步
    mutex m_Mutex;
    //解码器状态
    volatile int m_DecoderState = STATE_UNKNOWN;
    condition_variable m_Cond;
    double timeBase = 0;
    thread *m_DecodeThread = nullptr;

private:
    const AVCodec* m_AVCodec;
    AVCodecContext* m_AVCodecContext;
    AVFrame* m_Frame;
    ANativeWindow* m_NativeWindow;
    int m_VideoWidth;
    int m_VideoHeight;
    AVFrame* m_RGBAFrame;
    uint8_t* m_FrameBuffer;
    SwsContext* m_SwsContext;
    ANativeWindow_Buffer m_NativeWindowBuffer;

    void onDecoderReady();

    static void decodeThreadProc(VideoPlayer *player);

    void dealPackQueue();
};


#endif //VIDEOPLAYER_VIDEOPLAYER_H
