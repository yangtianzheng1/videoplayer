//
// Created by 杨天正 on 2022/9/11.
//

#ifndef VIDEOPLAYER_VIDEOPLAYER_H
#define VIDEOPLAYER_VIDEOPLAYER_H

#include <android/native_window.h>

extern "C"{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
}


class VideoPlayer {

public:
    int init(const char *url, ANativeWindow *window, int width, int height);
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
    ANativeWindow* m_NativeWindow;
    int m_VideoWidth;
    int m_VideoHeight;
    AVFrame *m_RGBAFrame;
    const uint8_t *m_FrameBuffer;
    SwsContext *m_SwsContext;
    SwrContext *m_SwrContext;
    ANativeWindow_Buffer m_NativeWindowBuffer;
};


#endif //VIDEOPLAYER_VIDEOPLAYER_H
