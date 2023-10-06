//
// Created by yangpc on 2023/10/6.
//

#ifndef VIDEOPLAYER_A_ENCODER_H
#define VIDEOPLAYER_A_ENCODER_H


#include "base_encoder.h"

extern "C" {
#include <libswresample/swresample.h>
};

class AudioEncoder: public BaseEncoder {

private:
    AVFrame *m_frame = NULL;

    void InitFrame();

protected:
    void InitContext(AVCodecContext *codec_ctx) override;

    int ConfigureMuxerStream(Mp4Muxer *muxer, AVCodecContext *ctx) override;

    AVFrame* DealFrame(OneFrame *one_frame) override;

    void Release() override;

    const char *const LogSpec() override {
        return "音频";
    };

public:
    AudioEncoder(JNIEnv *env, Mp4Muxer *muxer);

};


#endif //VIDEOPLAYER_A_ENCODER_H
