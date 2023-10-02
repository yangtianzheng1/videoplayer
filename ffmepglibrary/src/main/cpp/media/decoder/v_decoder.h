//
// Created by yangpc on 2023/10/1.
//

#ifndef VIDEOPLAYER_V_DECODER_H
#define VIDEOPLAYER_V_DECODER_H

#include "base_decoder.h"
#include "../render/video/video_render.h"
#include <jni.h>

extern "C"{
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
};

class VideoDecoder: public BaseDecoder{

private:
    const char *TAG = "VideoDecoder";

    const AVPixelFormat DST_FORMAT = AV_PIX_FMT_RGBA;

    AVFrame *m_rgb_frame = nullptr;

    uint8_t *m_buf_for_rgb_frame = nullptr;

    SwsContext *m_sws_ctx = nullptr;

    VideoRender *m_video_render = nullptr;

    int m_dst_w{};
    int m_dst_h{};

    void InitRender(JNIEnv *env);

    void InitBuffer();

    void InitSws();

public:
    VideoDecoder(JNIEnv *env, jstring path, bool for_synthesizer = false);

    ~VideoDecoder();

    void SetRender(VideoRender *render);


protected:
    AVMediaType GetMediaType() override{
        return AVMEDIA_TYPE_VIDEO;
    }

    bool NeedLoopDecode() override;

    void Prepare(JNIEnv *env) override;

    void Render(AVFrame *frame) override;

    void Release() override;

    const char *const LogSpec() override{
        return "VIDEO";
    }

};


#endif //VIDEOPLAYER_V_DECODER_H
