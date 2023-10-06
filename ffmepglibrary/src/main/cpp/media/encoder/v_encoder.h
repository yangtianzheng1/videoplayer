//
// Created by yangpc on 2023/10/6.
//

#ifndef VIDEOPLAYER_V_ENCODER_H
#define VIDEOPLAYER_V_ENCODER_H


#include "base_encoder.h"

class VideoEncoder : public BaseEncoder{

private:

    const char * TAG = "VideoEncoder";

    SwsContext *m_sws_ctx = nullptr;

    AVFrame *m_yuv_frame = nullptr;

    int m_width = 0, m_height = 0;

    void InitYUVFrame();

protected:
    const char *const LogSpec() override {
        return "视频";
    };

    void InitContext(AVCodecContext *codec_ctx) override;
    int ConfigureMuxerStream(Mp4Muxer *muxer, AVCodecContext *ctx) override;

    AVFrame* DealFrame(OneFrame *one_frame) override;
    void Release() override;

public:
    VideoEncoder(JNIEnv *env, Mp4Muxer *muxer, int width, int height);

};


#endif //VIDEOPLAYER_V_ENCODER_H
