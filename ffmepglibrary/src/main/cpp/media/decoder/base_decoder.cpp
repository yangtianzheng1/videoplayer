//
// Created by 杨天正 on 2023/9/17.
//

#include "base_decoder.h"
#include "libavutil/time.h"
#include "../one_frame.h"

BaseDecoder::BaseDecoder(JNIEnv *env, jstring path) {
    Init(env, path);
    CreateDecodeThread();
}

BaseDecoder::~BaseDecoder() {
    if (m_format_ctx != nullptr){
        delete m_format_ctx;
    }
    if (m_codec_ctx != nullptr){
        delete m_codec_ctx;
    }
    if (m_frame != nullptr){
        delete m_frame;
    }
    if (m_packet != nullptr){
        delete m_packet;
    }
}

void BaseDecoder::Init(JNIEnv *env, jstring path){
    m_path_ref = env->NewGlobalRef(path);
    m_path = env->GetStringUTFChars(path, nullptr);
    env->GetJavaVM(&m_jvm_for_thread);
}

void BaseDecoder::CreateDecodeThread() {
    // 使用智能指针，线程结束时，自动删除本类指针
    std::shared_ptr<BaseDecoder> that(this);
    std::thread t(Decode, that);
    t.detach();
}

void BaseDecoder::Decode(std::shared_ptr<BaseDecoder> that) {
    JNIEnv* env;

    //将线程附加到虚拟机，并获取env
    if (that->m_jvm_for_thread->AttachCurrentThread(&env, nullptr) != JNI_OK){
        LOG_ERROR(that->TAG, that->LogSpec(), "Fail to Init decode thread");
        return;
    }

    that->CallbackState(PREPARE);
    // 初始化解码器
    that->InitFFMpegDecoder(env);
    // 分配解码帧数据内存
    that->AllocFrameBuffer();
    av_usleep(1000);
    // 回调子类方法，通知子类解码器初始化完毕
    that->Prepare(env);
    // 进入解码循环
    that->LoopDecode();
    // 退出解码
    that->DoneDecode(env);

    that->CallbackState(STOP);

    //解除线程和jvm关联
    that->m_jvm_for_thread->DetachCurrentThread();
}

void BaseDecoder::InitFFMpegDecoder(JNIEnv *env) {
    //初始化上下文
    m_format_ctx = avformat_alloc_context();
    
    //打开文件
    if (avformat_open_input(&m_format_ctx, m_path, nullptr, nullptr) != 0){
        LOG_ERROR(TAG, LogSpec(), "Fail to open file [%s]", m_path);
        DoneDecode(env);
        return;
    }

    //3，获取音视频流信息
    if (avformat_find_stream_info(m_format_ctx, nullptr) < 0){
        LOG_ERROR(TAG, LogSpec(), "Fail to find stream info");
        DoneDecode(env);
        return;
    }

    //4，查找编解码器
    //4.1 获取视频流的索引
    int vIdx = -1;
    for (int i = 0; i < m_format_ctx->nb_streams; ++i) {
        if (m_format_ctx->streams[i]->codecpar->codec_type == GetMediaType()){
            vIdx = i;
            break;
        }
    }
    if (vIdx == -1) {
        LOG_ERROR(TAG, LogSpec(), "Fail to find stream index")
        DoneDecode(env);
        return;
    }

    m_stream_index = vIdx;
    //4.2 获取解码器参数
    AVCodecParameters *codecPar = m_format_ctx->streams[vIdx]->codecpar;
    //4.3 获取解码器
    m_codec = avcodec_find_decoder(codecPar->codec_id);

    //4.4 获取解码器上下文
    m_codec_ctx = avcodec_alloc_context3(m_codec);
    if (avcodec_parameters_to_context(m_codec_ctx, codecPar) != 0){
        LOG_ERROR(TAG, LogSpec(), "Fail to obtain av codec context");
        DoneDecode(env);
        return;
    }

    //5，打开解码器
    if (avcodec_open2(m_codec_ctx, m_codec, nullptr) < 0) {
        LOG_ERROR(TAG, LogSpec(), "Fail to open av codec");
        DoneDecode(env);
        return;
    }

    m_duration = (long)((float)m_format_ctx->duration/AV_TIME_BASE * 1000);

    LOG_INFO(TAG, LogSpec(), "Decoder init success")
}

void BaseDecoder::AllocFrameBuffer() {
    // 初始化待解码和解码数据结构
    // 1）初始化AVPacket，存放解码前的数据
    m_packet = av_packet_alloc();
    // 2）初始化AVFrame，存放解码后的数据
    m_frame = av_frame_alloc();
}

void BaseDecoder::LoopDecode() {
    if (STOP == m_state){
        m_state = START;
    }
    CallbackState(START);
    LOG_INFO(TAG, LogSpec(), "Start loop decode")

    while (true){
        if (m_state != DECODING &&
            m_state != START &&
            m_state != STOP) {
            Wait();
            // 恢复同步起始时间，去除等待流失的时间
            m_started_t = GetCurMsTime() - m_cur_t_s;
        }

        if (m_state == STOP) {
            break;
        }

        if (-1 == m_started_t) {
            m_started_t = GetCurMsTime();
        }

        if (DecodeOneFrame() != nullptr){
            SyncRender();
            Render(m_frame);

            if (m_state == START) {
                m_state = PAUSE;
            }
        } else{
            LOG_INFO(TAG, LogSpec(), "m_state = %d" ,m_state)
            if (ForSynthesizer()) {
                m_state = STOP;
            } else {
                m_state = FINISH;
            }
            CallbackState(FINISH);
        }
    }

}

AVFrame* BaseDecoder::DecodeOneFrame() {
    int ret = av_read_frame(m_format_ctx, m_packet);
    while (ret == 0){
        if (m_packet->stream_index == m_stream_index ){
            switch (avcodec_send_packet(m_codec_ctx, m_packet)){
                case AVERROR_EOF: {
                    av_packet_unref(m_packet);
                    LOG_ERROR(TAG, LogSpec(), "Decode error: %s", av_err2str(AVERROR_EOF));
                    return nullptr; //解码结束
                }
                case AVERROR(EAGAIN):
                    LOG_ERROR(TAG, LogSpec(), "Decode error: %s", av_err2str(AVERROR(EAGAIN)));
                    break;
                case AVERROR(EINVAL):
                    LOG_ERROR(TAG, LogSpec(), "Decode error: %s", av_err2str(AVERROR(EINVAL)));
                    break;
                case AVERROR(ENOMEM):
                    LOG_ERROR(TAG, LogSpec(), "Decode error: %s", av_err2str(AVERROR(ENOMEM)));
                    break;
                default:
                    break;
            }

            int result = avcodec_receive_frame(m_codec_ctx, m_frame);
            if (result == 0) {
                ObtainTimeStamp();
                av_packet_unref(m_packet);
                return m_frame;
            } else {
                LOG_INFO(TAG, LogSpec(), "Receive frame error result: %d", av_err2str(AVERROR(result)))
            }
        }
        // 释放packet
        av_packet_unref(m_packet);
        ret = av_read_frame(m_format_ctx, m_packet);
    }
    av_packet_unref(m_packet);
    LOGI(TAG, "ret = %s", av_err2str(AVERROR(ret)))
    return nullptr;
}

void BaseDecoder::CallbackState(DecodeState status) {
    if (m_state_cb != NULL) {
        switch (status) {
            case PREPARE:
                m_state_cb->DecodePrepare(this);
                break;
            case START:
                m_state_cb->DecodeReady(this);
                break;
            case DECODING:
                m_state_cb->DecodeRunning(this);
                break;
            case PAUSE:
                m_state_cb->DecodePause(this);
                break;
            case FINISH:
                m_state_cb->DecodeFinish(this);
                break;
            case STOP:
                m_state_cb->DecodeStop(this);
                break;
        }
    }
}




