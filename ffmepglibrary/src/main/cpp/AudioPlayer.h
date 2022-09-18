//
// Created by 杨天正 on 2022/9/12.
//

#ifndef VIDEOPLAYER_AUDIOPLAYER_H
#define VIDEOPLAYER_AUDIOPLAYER_H

#include "OpenSLRender.h"
#include "AVPacketQueue.h"
#include "MessageCallback.h"

extern "C"{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
#include <libavutil/opt.h>
#include <libavutil/time.h>
}

using namespace std;

// 音频编码采样率
static const int AUDIO_DST_SAMPLE_RATE = 44100;
// 音频编码通道数
static const int AUDIO_DST_CHANNEL_COUNTS = 2;
// 音频编码声道格式
static const uint64_t AUDIO_DST_CHANNEL_LAYOUT = AV_CH_LAYOUT_STEREO;
// 音频编码比特率
static const int AUDIO_DST_BIT_RATE = 64000;
// ACC音频一帧采样数
static const int ACC_NB_SAMPLES = 1024;

class AudioPlayer {

public:
    AudioPlayer(){};
    ~AudioPlayer(){};

    static  long  GetVideoDecoderTimestampForAVSync(void *audioDecoder){
        return  (reinterpret_cast<AudioPlayer *> (audioDecoder) )-> getCurrentPosition();
    }

    void init();
    void unInit();

    void start(AVFormatContext *avFormatContext);
    void pause();

    void resume();

    void stop();

    void clearCache();

    void UpdateTimeStamp();

    void AVSync();

    void setStreamIdx(int streamIdx) {
        m_StreamIdx = streamIdx;
    }

    int getStreamIdx() {
        return m_StreamIdx;
    }

    int getBufferSize() {
        return m_PacketQueue->GetSize();
    }

    long getCurrentPosition() {
        return m_CurTimeStamp;
    }

    void setMessageCallback(void *context, MessageCallback callback) {
        m_MsgContext = context;
        m_MsgCallback = callback;
    }

    void pushAVPacket(AVPacket *avPacket) {
        m_PacketQueue->PushPacket(avPacket);
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
    //解码器状态
    volatile int m_DecoderState = STATE_UNKNOWN;

    thread *m_DecodeThread = nullptr;
    //当前播放时间
    long m_CurTimeStamp = 0;
    std::mutex m_Mutex;
    std::condition_variable m_Cond;

    long m_StartTimeStamp = 0;

    void *m_AVDecoderContext = nullptr;
    AVSyncCallback m_AVSyncCallback = nullptr;//用作音视频同步

private:
    double timeBase = 0;
    const AVCodec* m_AVCodec = nullptr;
    const AVSampleFormat DST_SAMPLT_FORMAT = AV_SAMPLE_FMT_S16;
    const char* m_url;
    AVFormatContext* m_AVFormatContext = nullptr;
    int m_StreamIndex;
    AVCodecContext* m_AVCodecContext = nullptr;
    int64_t m_Duration;
    int m_nbSamples;
    //dst frame data size
    int m_DstFrameDataSze = 0;
    AVPacket* m_Packet = nullptr;
    AVFrame* m_Frame = nullptr;
    SwrContext *m_SwrContext = nullptr;
    int m_BufferSize;
    uint8_t* m_AudioOutBuffer = nullptr;
    OpenSLRender *openSlRender = nullptr;


    void onDecoderReady();

    static void decodeThreadProc(AudioPlayer* audioDecoder);

    void dealPackQueue();
};


#endif //VIDEOPLAYER_AUDIOPLAYER_H
