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

#define DELAY_THRESHOLD 100 //100ms

void VideoPlayer::init() {
    m_PacketQueue = new AVPacketQueue();
    m_PacketQueue->Start();
}

void VideoPlayer::unInit() {
    if (m_PacketQueue) {
        delete m_PacketQueue;
        m_PacketQueue = nullptr;
    }
}

//int VideoPlayer::init(const char* url, ANativeWindow* window, int width, int height){
//    m_url = url;
//    int result = -1;

//    m_AVFormatContext = avformat_alloc_context();
//    if (m_AVFormatContext == nullptr){
//        m_AVFormatContext = avformat_alloc_context();
//    }
//    if(avformat_open_input(&m_AVFormatContext, m_url, nullptr, nullptr) != 0){
//        LOGE("avformat_open_input fail");
//        return result;
//    }
//    if(avformat_find_stream_info(m_AVFormatContext, nullptr) < 0){
//        return result;
//    }
//    m_StreamIndex = av_find_best_stream(m_AVFormatContext,AVMEDIA_TYPE_VIDEO,-1,-1, nullptr, 0);
//    if(m_StreamIndex == -1){
//        return result;
//    }
//    AVCodecParameters* codecParameters = m_AVFormatContext->streams[m_StreamIndex]->codecpar;
//    m_AVCodec = avcodec_find_decoder(codecParameters->codec_id);
//    if(m_AVCodec == nullptr){
//        return result;
//    }
//    m_AVCodecContext = avcodec_alloc_context3(m_AVCodec);
//    if(avcodec_parameters_to_context(m_AVCodecContext, codecParameters) != 0){
//        return result;
//    }
//    AVDictionary *pAVDictionary = nullptr;
//    av_dict_set(&pAVDictionary, "buffer_size", "1024000", 0);
//    av_dict_set(&pAVDictionary, "stimeout", "20000000",0);
//    av_dict_set(&pAVDictionary, "max_delay", "30000000",0);
//    av_dict_set(&pAVDictionary,"rtsp_transport", "tcp",0);
//
//    result = avcodec_open2(m_AVCodecContext, m_AVCodec, &pAVDictionary);
//    if(result < 0){
//        return result;
//    }
//
//    m_Duration = m_AVFormatContext->duration/AV_TIME_BASE*1000;
//    m_Packet = av_packet_alloc();
//    m_Frame = av_frame_alloc();
//
//    m_NativeWindow = window;
//    ANativeWindow_setBuffersGeometry(m_NativeWindow, width, height, WINDOW_FORMAT_RGBA_8888);
//
//    m_VideoWidth = m_AVCodecContext->width;
//    m_VideoHeight = m_AVCodecContext->height;
//    m_RGBAFrame = av_frame_alloc();
//
//    int bufferSize = av_image_get_buffer_size(AV_PIX_FMT_RGBA, m_VideoWidth, m_VideoHeight, 1);
//    m_FrameBuffer = static_cast<uint8_t *>(av_malloc(bufferSize * sizeof (uint8_t)));
//    av_image_fill_arrays(m_RGBAFrame->data, m_RGBAFrame->linesize, m_FrameBuffer, AV_PIX_FMT_RGBA,
//                         m_VideoWidth, m_VideoHeight, 1);
//    m_SwsContext = sws_getContext(m_VideoWidth, m_VideoHeight, m_AVCodecContext->pix_fmt,
//                                  m_VideoWidth, m_VideoHeight, AV_PIX_FMT_RGBA,
//                                  SWS_BICUBIC, nullptr, nullptr, nullptr);

//    m_PacketQueue = new AVPacketQueue();
//    m_PacketQueue->Start();
//
//    return result;
//}

void VideoPlayer::start(AVFormatContext *m_AVFormatContext, ANativeWindow* window, int width, int height){
    m_StartTimeStamp = -1;
    unique_lock<mutex> lock(m_Mutex);

    m_DecoderState = STATE_DECODING;
    m_Cond.notify_all();

    int result = -1;
    do {
        timeBase = av_q2d(m_AVFormatContext->streams[m_StreamIdx]->time_base);
        AVCodecParameters  *codecParameters =
                m_AVFormatContext->streams[m_StreamIdx]->codecpar;
        m_AVCodec = avcodec_find_decoder(codecParameters->codec_id);
//        m_AVCodec = avcodec_find_decoder_by_name("h264_mediacodec");
        if (m_AVCodec == nullptr){
            LOGE("VideoDecoder::InitFFDecoder avcodec_find_decoder fail.");
            break;
        }
        m_AVCodecContext = avcodec_alloc_context3(m_AVCodec);
        if (avcodec_parameters_to_context(m_AVCodecContext, codecParameters) != 0){
            LOGE("VideoDecoder::InitFFDecoder avcodec_parameters_to_context fail.");
            break;
        }
        AVDictionary *pAVDictionary = nullptr;
        av_dict_set(&pAVDictionary, "buffer_size", "1024000", 0);
        av_dict_set(&pAVDictionary, "stimeout", "20000000", 0);
        av_dict_set(&pAVDictionary, "max_delay", "30000000", 0);
        av_dict_set(&pAVDictionary, "rtsp_transport", "tcp", 0);

        result = avcodec_open2(m_AVCodecContext, m_AVCodec,&pAVDictionary);
        if (result < 0){
            LOGE("VideoDecoder::InitFFDecoder avcodec_open2 fail. result=%d", result);
            break;
        }

        result = 0;
        m_Frame = av_frame_alloc();

    } while (false);

    onDecoderReady();

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

    if (result == 0) {
        m_DecodeThread = new thread(decodeThreadProc, this);
    }

//    int result = av_read_frame(m_AVFormatContext, m_Packet);
//    while (result >= 0){
//        if (m_Packet->stream_index == m_StreamIndex){
//            if (avcodec_send_packet(m_AVCodecContext, m_Packet) != 0){
//                return;
//            }
//            while (avcodec_receive_frame(m_AVCodecContext, m_Frame) == 0){
//                LOGD("m_Frame pts is %ld", m_Frame->pts);
//
//                sws_scale(m_SwsContext, m_Frame->data, m_Frame->linesize, 0,
//                          m_VideoHeight, m_RGBAFrame->data, m_RGBAFrame->linesize);
//                ANativeWindow_lock(m_NativeWindow, &m_NativeWindowBuffer, nullptr);
//                uint8_t *dstBuffer = static_cast<uint8_t *>(m_NativeWindowBuffer.bits);
//                int srcLineSize = m_RGBAFrame->linesize[0];
//                int dstLineSize = m_NativeWindowBuffer.stride * 4;
//                for (int i = 0; i < m_VideoHeight; ++i) {
//                    memcpy(dstBuffer + i * dstLineSize, m_FrameBuffer + i * srcLineSize, srcLineSize);
//                }
//                ANativeWindow_unlockAndPost(m_NativeWindow);
//            }
//        }
//        av_packet_unref(m_Packet);
//        result = av_read_frame(m_AVFormatContext, m_Packet);
//    }
}

void VideoPlayer::onDecoderReady() {
    m_VideoWidth = m_AVCodecContext->width;
    m_VideoHeight = m_AVCodecContext->height;

    if (m_MsgContext && m_MsgCallback){
        m_MsgCallback(m_MsgContext, MSG_DECODER_READY, 0);
    }
}

void VideoPlayer::pause() {
    unique_lock<mutex> lock(m_Mutex);
    m_DecoderState = STATE_PAUSE;
}

void VideoPlayer::resume() {
    unique_lock<mutex> lock(m_Mutex);
    m_DecoderState = STATE_DECODING;
    m_Cond.notify_all();
}

void VideoPlayer::stop() {
    unique_lock<mutex> lock(m_Mutex);
    m_DecoderState = STATE_STOP;
    m_Cond.notify_all();
    if (m_DecodeThread){
        m_DecodeThread->join();
        delete m_DecodeThread;
        m_DecodeThread = nullptr;
    }
    LOGE("VideoDecoder::Stop finish");

    if (m_PacketQueue){
        m_PacketQueue->Flush();
    }
    if(m_NativeWindow != nullptr){
        free(m_NativeWindow);
        m_NativeWindow = nullptr;
    }
    if (m_Frame != nullptr){
        av_frame_free(&m_Frame);
        m_Frame = nullptr;
    }
    if (m_AVCodecContext != nullptr){
        avcodec_close(m_AVCodecContext);
        avcodec_free_context(&m_AVCodecContext);
        m_AVCodecContext = nullptr;
        m_AVCodec = nullptr;
    }
    if (m_RGBAFrame != nullptr){
        av_frame_free(&m_RGBAFrame);
        m_RGBAFrame = nullptr;
    }
    if (m_FrameBuffer != nullptr){
        free(m_FrameBuffer);
        m_FrameBuffer = nullptr;
    }
    if (m_SwsContext != nullptr){
        sws_freeContext(m_SwsContext);
        m_SwsContext = nullptr;
    }
}

void VideoPlayer::decodeThreadProc(VideoPlayer *player) {
    player->dealPackQueue();
}

void VideoPlayer::dealPackQueue() {
    AVPacket *m_Packet = av_packet_alloc();
    while (m_DecoderState != STATE_STOP){
        while (m_DecoderState == STATE_PAUSE){
            unique_lock<mutex> lock(m_Mutex);
            m_Cond.wait_for(lock, std::chrono::milliseconds(10));
            m_StartTimeStamp = GetSysCurrentTime() - m_CurTimeStamp;
        }
        if(m_StartTimeStamp == -1){
            m_StartTimeStamp = GetSysCurrentTime();
        }
        if (m_DecoderState == STATE_STOP) {
            LOGE("VideoDecoder::DecodingLoop stop break thread");
            goto __EXIT;
        }
        (m_PacketQueue->GetPacketSize() == 0);

        if (m_PacketQueue->GetPacket(m_Packet) < 0) {
            goto __EXIT;
        }
        if (avcodec_send_packet(m_AVCodecContext, m_Packet) == AVERROR_EOF) {
            //解码结束
            goto __EXIT;
        }
        //一个 packet 包含多少 frame?
        int frameCount = 0;
        LOGE("UpdateTimeStamp   VideoDecoder m_Packet %ld",m_Packet->pts);
        while (avcodec_receive_frame(m_AVCodecContext, m_Frame) >= 0){
            //同步
            UpdateTimeStamp();
            AVSync();
            //渲染
            LOGE("VideoDecoder::DecodeOnePacket 000 m_MediaType=%d");
            LOGE("VideoDecoder::OnFrameAvailable frame=%p", m_Frame);
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

            if (m_MsgContext && m_MsgCallback){
                m_MsgCallback(m_MsgContext, MSG_REQUEST_RENDER, 0);
            }
            LOGE("VideoDecoder::DecodeOnePacket 0001 m_MediaType=%d");
            frameCount++;
            LOGE("BaseDecoder::DecodeOneFrame frameCount=%d", frameCount);
        }
        av_packet_unref(m_Packet);
    }
    __EXIT:
    av_packet_unref(m_Packet);
    free(m_Packet);
    m_Packet = nullptr;
}

void VideoPlayer::UpdateTimeStamp() {
    if (m_Frame->pkt_dts != AV_NOPTS_VALUE) {
        m_CurTimeStamp = m_Frame->pkt_dts;
    } else if (m_Frame->pts != AV_NOPTS_VALUE) {
        m_CurTimeStamp = m_Frame->pts;
    } else {
        m_CurTimeStamp = 0;
    }
    m_CurTimeStamp = (int64_t)(m_CurTimeStamp * timeBase * 1000);
    LOGE("UpdateTimeStamp   VideoDecoder %ld %ld %ld",m_Frame->pts,m_Frame->pkt_dts,m_CurTimeStamp);
}

void VideoPlayer::AVSync() {
    LOGE("VideoDecoder::AVSync");
    if (m_AVSyncCallback != nullptr) {
        //视频向音频同步,传进来的 m_AVSyncCallback 用于获取音频时间戳
        long elapsedTime = m_AVSyncCallback(m_AVDecoderContext);
        LOGE("VideoDecoder::AVSync m_CurTimeStamp=%ld, elapsedTime=%ld", m_CurTimeStamp,
                elapsedTime);

        if (m_CurTimeStamp > elapsedTime) {
            //休眠时间
            auto sleepTime = static_cast<unsigned int>(m_CurTimeStamp - elapsedTime);//ms
            av_usleep(sleepTime * 1000);
        }
    }

//    long curSysTime = GetSysCurrentTime();
//    //基于系统时钟计算从开始播放流逝的时间
//    long elapsedTime = curSysTime - m_StartTimeStamp;
//
//    if(m_MsgContext && m_MsgCallback)
//        m_MsgCallback(m_MsgContext, MSG_DECODING_TIME, m_CurTimeStamp * 1.0f / 1000);
//
//    long delay = 0;
//    //向系统时钟同步
//    if(m_CurTimeStamp > elapsedTime) {
//        //休眠时间
//        auto sleepTime = static_cast<unsigned int>(m_CurTimeStamp - elapsedTime);//ms
//        //限制休眠时间不能过长
//        sleepTime = sleepTime > DELAY_THRESHOLD ? DELAY_THRESHOLD :  sleepTime;
//        av_usleep(sleepTime * 1000);
//    }
//    delay = elapsedTime - m_CurTimeStamp;
}

void VideoPlayer::clearCache() {
    unique_lock<mutex> vBufLock(m_Mutex);
    m_PacketQueue->Flush();
    avcodec_flush_buffers(m_AVCodecContext);
    vBufLock.unlock();
}