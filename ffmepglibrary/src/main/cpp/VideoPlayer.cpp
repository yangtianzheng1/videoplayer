//
// Created by 杨天正 on 2022/9/11.
//
#include <stdio.h>
#include <string.h>
#include <android/log.h>
#include "VideoPlayer.h"

#define TAG "VideoDecoder" // 这个是自定义的LOG的标识
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG,TAG ,__VA_ARGS__) // 定义LOGD类型
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,TAG ,__VA_ARGS__) // 定义LOGI类型
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN,TAG ,__VA_ARGS__) // 定义LOGW类型
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,TAG ,__VA_ARGS__) // 定义LOGE类型
#define LOGF(...) __android_log_print(ANDROID_LOG_FATAL,TAG ,__VA_ARGS__) // 定义LOGF类型

int VideoPlayer::init(const char* url, ANativeWindow* window, int width, int height){
    m_url = url;
    int result = -1;

    m_AVFormatContext = avformat_alloc_context();
    if (m_AVFormatContext == nullptr){
        m_AVFormatContext = avformat_alloc_context();
    }
    if(avformat_open_input(&m_AVFormatContext, m_url, nullptr, nullptr) != 0){
        LOGE("avformat_open_input fail");
        return result;
    }
    if(avformat_find_stream_info(m_AVFormatContext, nullptr) < 0){
        return result;
    }
    m_StreamIndex = av_find_best_stream(m_AVFormatContext,AVMEDIA_TYPE_VIDEO,-1,-1, nullptr, 0);
    if(m_StreamIndex == -1){
        return result;
    }
    AVCodecParameters* codecParameters = m_AVFormatContext->streams[m_StreamIndex]->codecpar;
    m_AVCodec = avcodec_find_decoder(codecParameters->codec_id);
    if(m_AVCodec == nullptr){
        return result;
    }
    m_AVCodecContext = avcodec_alloc_context3(m_AVCodec);
    if(avcodec_parameters_to_context(m_AVCodecContext, codecParameters) != 0){
        return result;
    }
    AVDictionary *pAVDictionary = nullptr;
    av_dict_set(&pAVDictionary, "buffer_size", "1024000", 0);
    av_dict_set(&pAVDictionary, "stimeout", "20000000",0);
    av_dict_set(&pAVDictionary, "max_delay", "30000000",0);
    av_dict_set(&pAVDictionary,"rtsp_transport", "tcp",0);

    result = avcodec_open2(m_AVCodecContext, m_AVCodec, &pAVDictionary);
    if(result < 0){
        return result;
    }

    m_Duration = m_AVFormatContext->duration/AV_TIME_BASE*1000;
    m_Packet = av_packet_alloc();
    m_Frame = av_frame_alloc();

    m_NativeWindow = window;
    ANativeWindow_setBuffersGeometry(m_NativeWindow, width, height, WINDOW_FORMAT_RGBA_8888);

    m_VideoWidth = m_AVCodecContext->width;
    m_VideoHeight = m_AVCodecContext->height;
    m_RGBAFrame = av_frame_alloc();

    int bufferSize = av_image_get_buffer_size(AV_PIX_FMT_RGBA, m_VideoWidth, m_VideoHeight, 1);
    m_FrameBuffer = static_cast<uint8_t *>(av_malloc(bufferSize * sizeof (uint8_t)));
    av_image_fill_arrays(m_RGBAFrame->data, m_RGBAFrame->linesize, m_FrameBuffer, AV_PIX_FMT_RGBA,
                         m_VideoWidth, m_VideoHeight, 1);
    m_SwsContext = sws_getContext(m_VideoWidth, m_VideoHeight, m_AVCodecContext->pix_fmt,
                                  m_VideoWidth, m_VideoHeight, AV_PIX_FMT_RGBA,
                                  SWS_BICUBIC, nullptr, nullptr, nullptr);

    return result;
}

void VideoPlayer::start(){
    int result = av_read_frame(m_AVFormatContext, m_Packet);
    while (result >= 0){
        if (m_Packet->stream_index == m_StreamIndex){
            if (avcodec_send_packet(m_AVCodecContext, m_Packet) != 0){
                return;
            }
            while (avcodec_receive_frame(m_AVCodecContext, m_Frame) == 0){
                LOGD("m_Frame pts is %ld", m_Frame->pts);

                sws_scale(m_SwsContext, m_Frame->data, m_Frame->linesize, 0,
                          m_VideoHeight, m_RGBAFrame->data, m_RGBAFrame->linesize);
                ANativeWindow_lock(m_NativeWindow, &m_NativeWindowBuffer, nullptr);
                uint8_t *dstBuffer = static_cast<uint8_t *>(m_NativeWindowBuffer.bits);
                int srcLineSize = m_RGBAFrame->linesize[0];
                int dstLineSize = m_NativeWindowBuffer.stride * 4;
                for (int i = 0; i < m_VideoHeight; ++i) {
                    memcpy(dstBuffer + i * dstLineSize, m_FrameBuffer + i * srcLineSize, srcLineSize);
                }
                ANativeWindow_unlockAndPost(m_NativeWindow);
            }
        }
        av_packet_unref(m_Packet);
        result = av_read_frame(m_AVFormatContext, m_Packet);
    }
}

void VideoPlayer::release(){
    if (m_Frame != nullptr){
        av_frame_free(&m_Frame);
        m_Frame = nullptr;
    }
    if (m_Packet != nullptr){
        av_packet_free(&m_Packet);
        m_Packet = nullptr;
    }
    if (m_AVCodecContext != nullptr){
        avcodec_close(m_AVCodecContext);
        avcodec_free_context(&m_AVCodecContext);
        m_AVCodecContext = nullptr;
        m_AVCodec = nullptr;
    }
    if (m_AVFormatContext != nullptr){
        avformat_close_input(&m_AVFormatContext);
        avformat_free_context(m_AVFormatContext);
        m_AVFormatContext = nullptr;
    }
}