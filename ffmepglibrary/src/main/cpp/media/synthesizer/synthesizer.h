//
// Created by yangpc on 2023/10/6.
//

#ifndef VIDEOPLAYER_SYNTHESIZER_H
#define VIDEOPLAYER_SYNTHESIZER_H


#include "../muxer/i_muxer_cb.h"
#include "../decoder/i_decode_state_cb.h"
#include "../render/video/opengl_pixel_receiver.h"
#include "../encoder/i_encoder.h"
#include "../decoder/v_decoder.h"
#include "../decoder/a_decoder.h"
#include "../render/video/opengl_render.h"
#include "../muxer/mp4_muxer.h"
#include "../encoder/v_encoder.h"
#include "../encoder/a_encoder.h"

extern "C"{
#include <libavutil/time.h>
};

class Synthesizer : IMuxerCb, IDecodeStateCb, IEncodeStateCb, OpenGLPixelReceiver{

private:

    VideoDecoder *m_video_decoder = nullptr;

    AudioDecoder *m_audio_decoder = nullptr;

    OpenglRender *m_gl_render = nullptr;

    DrawerProxy *m_drawer_proxy = nullptr;

    Mp4Muxer *m_mp4_muxer = nullptr;

    VideoEncoder *m_v_encoder = nullptr;

    AudioEncoder *m_a_encoder = nullptr;

    OneFrame *m_cur_v_frame = nullptr;
    OneFrame *m_cur_a_frame = nullptr;

public:
    Synthesizer(JNIEnv *env, jstring src_path, jstring dst_path);
    ~Synthesizer();

    void Start();

    void ReceivePixel(uint8_t *data) override ;
    void OnMuxFinished() override;

    void DecodePrepare(IDecoder *decoder) override;
    void DecodeReady(IDecoder *decoder) override;
    void DecodeRunning(IDecoder *decoder) override;
    void DecodePause(IDecoder *decoder) override;
    bool DecodeOneFrame(IDecoder *decoder, OneFrame *frame) override;
    void DecodeFinish(IDecoder *decoder) override;
    void DecodeStop(IDecoder *decoder) override;

    void EncodeStart() override;

    void EncodeSend() override;

    void EncodeFrame(void *data) override;
    void EncodeProgress(long time) override;
    void EncodeFinish() override;
};


#endif //VIDEOPLAYER_SYNTHESIZER_H
