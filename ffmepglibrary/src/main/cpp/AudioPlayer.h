//
// Created by 杨天正 on 2022/9/12.
//

#ifndef VIDEOPLAYER_AUDIOPLAYER_H
#define VIDEOPLAYER_AUDIOPLAYER_H

#include "OpenSLRender.h"

extern "C"{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
#include <libavutil/opt.h>
}


class AudioPlayer {

public:
    int init(const char *url);
    void start();
    void release();

private:
    const int64_t AUDIO_DST_CHANNEL_LAYOUT = AV_CH_LAYOUT_STEREO;
    const int64_t AUDIO_DST_SAMPLE_RATE = 44100;
    const int64_t NB_SAMPLES = 1024;
    const int AUDIO_DST_CHANNEL_COUNTS = 2;
    const AVCodec* m_AVCodec;
    const char* m_url;
    AVFormatContext* m_AVFormatContext;
    int m_StreamIndex;
    AVCodecContext* m_AVCodecContext;
    int64_t m_Duration;
    int m_nbSamples;
    AVPacket* m_Packet;
    AVFrame* m_Frame;
    SwrContext *m_SwrContext;
    int m_BufferSize;
    uint8_t* m_AudioOutBuffer;
    OpenSLRender *openSlRender = nullptr;
};


#endif //VIDEOPLAYER_AUDIOPLAYER_H
