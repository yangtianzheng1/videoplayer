//
// Created by 杨天正 on 2022/9/11.
//

#ifndef VIDEOPLAYER_VIDEOPLAYER_H
#define VIDEOPLAYER_VIDEOPLAYER_H

extern "C"{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
}


class VideoPlayer {

public:
    int init(const char *url);
    void start();
    void release();

private:
    const AVCodec* m_AVCodec;
    const char* m_url;
    AVFormatContext* m_AVFormatContext;
    int m_StreamIndex;
    AVCodecContext* m_AVCodecContext;
    int64_t m_Duration;
    AVPacket* m_Packet;
    AVFrame* m_Frame;
};


#endif //VIDEOPLAYER_VIDEOPLAYER_H
