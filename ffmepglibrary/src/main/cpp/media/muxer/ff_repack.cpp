//
// Created by yangpc on 2023/10/4.
//

#include <unistd.h>
#include "ff_repack.h"
#include "../../utils/logger.h"

FFRepack::FFRepack(JNIEnv *env, jstring in_path, jstring out_path) {
    const char *srcPath = env->GetStringUTFChars(in_path, nullptr);
    const char *destPath = env->GetStringUTFChars(out_path, nullptr);

    if (OpenSrcFile(srcPath) >= 0){
        if (InitMuxerParams(destPath)){
            LOGE(TAG, "Init muxer params fail")
        }else {
            LOGE(TAG, "Init muxer params fail")
        }
    }else{
        LOGE(TAG, "Open src file fail")
    }
}

int FFRepack::OpenSrcFile(const char *srcPath) {
    if ((avformat_open_input(&m_in_format_cxt, srcPath, nullptr, nullptr)) < 0) {
        LOGE(TAG, "Fail to open input file")
        return -1;
    }
    if (avformat_find_stream_info(m_in_format_cxt, nullptr) < 0){
        LOGE(TAG, "Fail to retrieve input stream information")
        return -1;
    }
    return 0;
}

int FFRepack::InitMuxerParams(const char *destPath) {
    if (avformat_alloc_output_context2(&m_out_format_cxt, nullptr, nullptr, destPath) < 0) {
        return -1;
    }

    for (int i = 0; i < m_in_format_cxt->nb_streams; ++i) {
        AVStream *in_stream = m_in_format_cxt->streams[i];

        AVStream *out_stream = avformat_new_stream(m_out_format_cxt, nullptr);

        if (!out_stream){
            LOGE(TAG, "Fail to allocate output stream")
            return -1;
        }

        if (avcodec_parameters_copy(out_stream->codecpar, in_stream->codecpar) < 0){
            LOGE(TAG, "Fail to copy input context to output stream")
            return -1;
        }
    }

    if (avio_open(&m_out_format_cxt->pb, destPath, AVIO_FLAG_WRITE) < 0){
        LOGE(TAG, "Could not open output file %s ", destPath);
        return -1;
    }

    if (avformat_write_header(m_out_format_cxt, nullptr) < 0){
        LOGE(TAG, "Error occurred when opening output file");
        return -1;
    }else{
        LOGE(TAG, "Write file header success");
    }

    return 0;
}

void FFRepack::Start() {
    LOGE(TAG, "Start repacking ....")
    AVPacket pkt;
    while (true){
        if (av_read_frame(m_in_format_cxt, &pkt)){
            LOGE(TAG, "End of video，write trailer")
            // 释放数据帧
            av_packet_unref(&pkt);
            // 读取完毕，写入结尾信息
            av_write_trailer(m_out_format_cxt);
            break;
        }

        Write(pkt);
    }

    Release();
}

void FFRepack::Write(AVPacket avPacket) {
    AVStream  *in_stream = m_in_format_cxt->streams[avPacket.stream_index];
    AVStream *out_stream = m_out_format_cxt->streams[avPacket.stream_index];

    int rounding = (AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX);

    avPacket.pts = av_rescale_q_rnd(avPacket.pts, in_stream->time_base, out_stream->time_base, (AVRounding)rounding);

    avPacket.dts = av_rescale_q_rnd(avPacket.dts, in_stream->time_base, out_stream->time_base, (AVRounding)rounding);

    avPacket.duration = av_rescale_q(avPacket.duration, in_stream->time_base, out_stream->time_base);

    avPacket.pos = -1;

    int ret = av_interleaved_write_frame(m_out_format_cxt, &avPacket);
    if ( ret < 0){
        LOGE(TAG, "Error to muxing packet: %x", ret);
    }

}

void FFRepack::Release() {
    LOGE(TAG, "Finish repacking, release resources");

    if (m_in_format_cxt) {
        avformat_close_input(&m_in_format_cxt);
    }

    if (m_out_format_cxt) {
        avio_close(m_out_format_cxt->pb);
        avformat_free_context(m_out_format_cxt);
    }
}
