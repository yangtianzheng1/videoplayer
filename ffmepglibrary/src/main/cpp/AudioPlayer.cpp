//
// Created by 杨天正 on 2022/9/12.
//

#include <cstdio>
#include <cstring>
#include <android/log.h>
#include "AudioPlayer.h"
#include "OpenSLRender.h"

#define TAG "VideoDecoder" // 这个是自定义的LOG的标识
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG,TAG ,__VA_ARGS__) // 定义LOGD类型
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,TAG ,__VA_ARGS__) // 定义LOGI类型
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN,TAG ,__VA_ARGS__) // 定义LOGW类型
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,TAG ,__VA_ARGS__) // 定义LOGE类型
#define LOGF(...) __android_log_print(ANDROID_LOG_FATAL,TAG ,__VA_ARGS__) // 定义LOGF类型

#define DELAY_THRESHOLD 100 //100ms

void AudioPlayer::init() {
    openSlRender = new OpenSLRender();
    openSlRender->init();
    m_PacketQueue = new AVPacketQueue();
    m_PacketQueue->Start();
}

void AudioPlayer::unInit() {
    openSlRender->unInit();
    openSlRender = nullptr;

    if (m_PacketQueue) {
        delete m_PacketQueue;
        m_PacketQueue = nullptr;
    }
}

void AudioPlayer::start(AVFormatContext *avFormatContext) {

    m_DecoderState = STATE_DECODING;
    int result = -1;
    do{
        timeBase = av_q2d(avFormatContext->streams[m_StreamIdx]->time_base);
        //5.获取解码器参数
        AVCodecParameters *codecParameters = avFormatContext->streams[m_StreamIdx]->codecpar;
        //6.获取解码器
        m_AVCodec = avcodec_find_decoder(codecParameters->codec_id);
        if (m_AVCodec == nullptr) {
            LOGE("AudioDecoder::InitFFDecoder avcodec_find_decoder fail.");
            break;
        }
        //7.创建解码器上下文
        m_AVCodecContext = avcodec_alloc_context3(m_AVCodec);
        if (avcodec_parameters_to_context(m_AVCodecContext, codecParameters) != 0) {
            LOGE("AudioDecoder::InitFFDecoder avcodec_parameters_to_context fail.");
            break;
        }
        AVDictionary *pAVDictionary = nullptr;
        av_dict_set(&pAVDictionary, "buffer_size", "1024000", 0);
        av_dict_set(&pAVDictionary, "stimeout", "20000000", 0);
        av_dict_set(&pAVDictionary, "max_delay", "30000000", 0);
        av_dict_set(&pAVDictionary, "rtsp_transport", "tcp", 0);

        //8.打开解码器
        result = avcodec_open2(m_AVCodecContext, m_AVCodec, &pAVDictionary);
        if (result < 0) {
            LOGE("AudioDecoder::InitFFDecoder avcodec_open2 fail. result=%d", result);
            break;
        }
        result = 0;

        //创建 AVFrame 存放解码后的数据
        m_Frame = av_frame_alloc();

    } while (false);

    onDecoderReady();
    if (result == 0) {
        m_DecodeThread = new thread(decodeThreadProc, this);
    }
}

void AudioPlayer::onDecoderReady() {
    m_SwrContext = swr_alloc();

    av_opt_set_int(m_SwrContext, "in_channel_layout", m_AVCodecContext->channel_layout, 0);
    av_opt_set_int(m_SwrContext, "out_channel_layout", AUDIO_DST_CHANNEL_LAYOUT, 0);

    av_opt_set_int(m_SwrContext, "in_sample_rate", m_AVCodecContext->sample_rate, 0);
    av_opt_set_int(m_SwrContext, "out_sample_rate", AUDIO_DST_SAMPLE_RATE, 0);

    av_opt_set_sample_fmt(m_SwrContext, "in_sample_fmt", m_AVCodecContext->sample_fmt, 0);
    av_opt_set_sample_fmt(m_SwrContext, "out_sample_fmt", DST_SAMPLT_FORMAT, 0);

    swr_init(m_SwrContext);
    LOGE("AudioDecoder::OnDecoderReady audio metadata sample rate: %d, channel: %d, format: %d, frame_size: %d, layout: %lld",
            m_AVCodecContext->sample_rate, m_AVCodecContext->channels, m_AVCodecContext->sample_fmt,
            m_AVCodecContext->frame_size, m_AVCodecContext->channel_layout);
    // resample params
    m_nbSamples = (int) av_rescale_rnd(ACC_NB_SAMPLES, AUDIO_DST_SAMPLE_RATE,
                                       m_AVCodecContext->sample_rate, AV_ROUND_UP);
    m_DstFrameDataSze = av_samples_get_buffer_size(nullptr, AUDIO_DST_CHANNEL_COUNTS, m_nbSamples,
                                                   DST_SAMPLT_FORMAT, 1);
    LOGE("AudioDecoder::OnDecoderReady [m_nbSamples, m_DstFrameDataSze]=[%d, %d]", m_nbSamples,
            m_DstFrameDataSze);
    m_AudioOutBuffer = (uint8_t *) malloc(m_DstFrameDataSze);
}

void AudioPlayer::pause() {
    unique_lock<mutex> lock(m_Mutex);
    m_DecoderState = STATE_PAUSE;
}

void AudioPlayer::resume() {
    unique_lock<mutex> lock(m_Mutex);
    m_DecoderState = STATE_DECODING;
    m_Cond.notify_all();
}

void AudioPlayer::stop() {
    LOGE("AudioDecoder::Stop");
    unique_lock<mutex> lock(m_Mutex);
    m_DecoderState = STATE_STOP;
    m_Cond.notify_all();

    if (m_DecodeThread) {
        m_DecodeThread->join();
        delete m_DecodeThread;
        m_DecodeThread = nullptr;
    }
    if (m_PacketQueue) {
        m_PacketQueue->Flush();
    }
    LOGE("AudioDecoder::Stop finish");
    if (m_Frame != nullptr) {
        av_frame_free(&m_Frame);
        m_Frame = nullptr;
    }
    if (m_AVCodecContext != nullptr) {
        avcodec_close(m_AVCodecContext);
        avcodec_free_context(&m_AVCodecContext);
        m_AVCodecContext = nullptr;
        m_AVCodec = nullptr;
    }
    if (m_AudioOutBuffer) {
        free(m_AudioOutBuffer);
        m_AudioOutBuffer = nullptr;
    }
    if (m_SwrContext) {
        swr_free(&m_SwrContext);
        m_SwrContext = nullptr;
    }
}

void AudioPlayer::decodeThreadProc(AudioPlayer *audioDecoder) {
    audioDecoder->dealPackQueue();
}

void AudioPlayer::dealPackQueue(){
    AVPacket *mPacket = av_packet_alloc();
    while (m_DecoderState != STATE_STOP){
        while (m_DecoderState == STATE_PAUSE){
            unique_lock<mutex> lock(m_Mutex);
            m_Cond.wait_for(lock, chrono::milliseconds(10));
        }
        if (m_DecoderState == STATE_STOP) {
             LOGE("AudioDecoder::DecodingLoop stop break thread ");
            goto __EXIT;
        }
        (m_PacketQueue->GetPacketSize() == 0);
        if (m_PacketQueue->GetPacket(mPacket) < 0) {
            goto __EXIT;
        }
        if (avcodec_send_packet(m_AVCodecContext, mPacket) == AVERROR_EOF) {
            //解码结束
            goto __EXIT;
        }
        //一个 packet 包含多少 frame?
        int frameCount = 0;
        while (avcodec_receive_frame(m_AVCodecContext, m_Frame) >= 0){
            //同步
            UpdateTimeStamp();
            AVSync();
            if (openSlRender) {
                int result = swr_convert(m_SwrContext, &m_AudioOutBuffer,
                                         m_DstFrameDataSze / 2,
                                         (const uint8_t **) m_Frame->data,
                                         m_Frame->nb_samples);
                if (result > 0) {
                    openSlRender->RenderAudioFrame(m_AudioOutBuffer, m_DstFrameDataSze);
                }
            }
            frameCount++;
        }
        av_packet_unref(mPacket);
    }
    __EXIT:
    av_packet_unref(mPacket);
    free(mPacket);
    mPacket = nullptr;
}

void AudioPlayer::AVSync(){
    long curSysTime = GetSysCurrentTime();
    //基于系统时钟计算从开始播放流逝的时间
    long elapsedTime = curSysTime - m_StartTimeStamp;

    if(m_MsgContext && m_MsgCallback)
        m_MsgCallback(m_MsgContext, MSG_DECODING_TIME, m_CurTimeStamp * 1.0f / 1000);

//    if(m_AVSyncCallback != nullptr) {
//        //视频向音频同步,传进来的 m_AVSyncCallback 用于获取音频时间戳
//        long syncCallback = m_AVSyncCallback(m_AVDecoderContext);
//        //   LOGCATE("AudioDecoder::AVSync m_CurTimeStamp=%ld, syncCallback=%ld", m_CurTimeStamp, syncCallback);
//        if(m_CurTimeStamp > syncCallback) {
//            //休眠时间
//            auto sleepTime = static_cast<unsigned int>(m_CurTimeStamp - syncCallback);//ms
//            av_usleep(sleepTime * 1000);
//        }
//    }

    long delay = 0;
    //向系统时钟同步
    if(m_CurTimeStamp > elapsedTime) {
        //休眠时间
        auto sleepTime = static_cast<unsigned int>(m_CurTimeStamp - elapsedTime);//ms
        //限制休眠时间不能过长
        sleepTime = sleepTime > DELAY_THRESHOLD ? DELAY_THRESHOLD :  sleepTime;
        av_usleep(sleepTime * 1000);
    }
    delay = elapsedTime - m_CurTimeStamp;
}

void AudioPlayer::UpdateTimeStamp(){
    if (m_Frame->pkt_dts != AV_NOPTS_VALUE) {
        m_CurTimeStamp = m_Frame->pkt_dts;
    } else if (m_Frame->pts != AV_NOPTS_VALUE) {
        m_CurTimeStamp = m_Frame->pts;
    } else {
        m_CurTimeStamp = 0;
    }
    m_CurTimeStamp = (int64_t) (m_CurTimeStamp * timeBase * 1000);
    m_StartTimeStamp = GetSysCurrentTime() - m_CurTimeStamp;
}

void AudioPlayer::clearCache() {
    unique_lock<mutex> vBufLock(m_Mutex);
    m_PacketQueue->Flush();
    avcodec_flush_buffers(m_AVCodecContext);
    openSlRender->ClearAudioCache();
    vBufLock.unlock();
}
