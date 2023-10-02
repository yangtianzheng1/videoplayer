//
// Created by yangpc on 2023/10/2.
//

#ifndef VIDEOPLAYER_A_DECODER_H
#define VIDEOPLAYER_A_DECODER_H

#include "base_decoder.h"
#include "../render/audio/audio_render.h"
#include "../const.h"

extern "C"{
#include <libswresample/swresample.h>
#include <libavutil/opt.h>
};

class AudioDecoder : public BaseDecoder{

private:
    const char *TAG = "AudioDecoder";
    SwrContext *m_swr = nullptr;

    AudioRender *m_render = nullptr;

    uint8_t *m_out_buffer[2] = {nullptr, nullptr};

    int m_dest_nb_sample = 1024;

    size_t m_dest_data_size = 0;

    void InitSwr();

    /**
 * 计算重采样后通道采样数和帧数据大小
 */
    void CalculateSampleArgs();

    void InitOutBuffer();

    void InitRender();

    void ReleaseOutBuffer();

    AVSampleFormat GetSampleFmt(){
        if (ForSynthesizer()) {
            return ENCODE_AUDIO_DEST_FORMAT;
        } else {
            return AV_SAMPLE_FMT_S16;
        }
    }

    int GetSampleRate(int spr){
        if (ForSynthesizer()) {
            return ENCODE_AUDIO_DEST_SAMPLE_RATE;
        } else {
            return spr;
        }
    }


public:

    AudioDecoder(JNIEnv *env, const jstring path, bool forSynthesizer);

    ~AudioDecoder();

    void SetRender(AudioRender *render);

protected:
    void Prepare(JNIEnv *env) override;

    void Render(AVFrame *frame) override;

    void Release() override;


    bool NeedLoopDecode() override{
        return true;
    }

    AVMediaType GetMediaType() override{
        return AVMEDIA_TYPE_AUDIO;
    }

    const char *const LogSpec() override {
        return "AUDIO";
    }
};

#endif //VIDEOPLAYER_A_DECODER_H
