//
// Created by yangpc on 2023/10/2.
//

#ifndef VIDEOPLAYER_OPENSL_RENDER_H
#define VIDEOPLAYER_OPENSL_RENDER_H

#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
#include <queue>
#include <string>
#include <pthread.h>
#include "audio_render.h"

extern "C"{
#include <libavutil/mem.h>
};

static const char *TAG = "OpenSLRender";

class OpenSLRender1 : public AudioRender{

private:
    class PcmData{
    public:
        PcmData(uint8_t *pcm, int size) {
            this->pcm = pcm;
            this->size = size;
        }
        ~PcmData() {
            if (pcm != nullptr) {
                //释放已使用的内存
                free(pcm);
                pcm = nullptr;
                used = false;
            }
        }
        uint8_t *pcm = NULL;
        int size = 0;
        bool used = false;
    };
    const SLuint32 SL_QUEUE_BUFFER_COUNT = 2;

    SLObjectItf m_engine_obj = nullptr;
    SLEngineItf m_engine = nullptr;

    //混音器
    SLObjectItf m_output_mix_obj = nullptr;
    SLEnvironmentalReverbItf m_output_mix_evn_reverb = nullptr;
    SLEnvironmentalReverbSettings m_reverb_settings = SL_I3DL2_ENVIRONMENT_PRESET_DEFAULT;

    //pcm播放器
    SLObjectItf m_pcm_player_obj = nullptr;
    SLPlayItf m_pcm_player = nullptr;
    SLVolumeItf m_pcm_player_volume = nullptr;

    //缓冲器队列接口
    SLAndroidSimpleBufferQueueItf m_pcm_buffer{};

    std::queue<PcmData *> m_data_queue;

    // 缓存线程等待锁变量
    pthread_mutex_t m_cache_mutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_cond_t m_cache_cond = PTHREAD_COND_INITIALIZER;

    bool CreateEngine();
    bool CreateOutputMixer();
    bool ConfigPlayer();
    void StartRender();
    void BlockEnqueue();
    static bool CheckError(SLresult result, std::string hint);

    void static sRenderPcm(OpenSLRender1 *that);
    void static sReadPcmBufferCbFun(SLAndroidSimpleBufferQueueItf bufferQueueItf, void *context);

    void WaitForCache() {
        pthread_mutex_lock(&m_cache_mutex);
        pthread_cond_wait(&m_cache_cond, &m_cache_mutex);
        pthread_mutex_unlock(&m_cache_mutex);
    }

    void SendCacheReadySignal() {
        pthread_mutex_lock(&m_cache_mutex);
        pthread_cond_signal(&m_cache_cond);
        pthread_mutex_unlock(&m_cache_mutex);
    }

public:
    OpenSLRender1();
    ~OpenSLRender1();

    void InitRender() override;
    void Render(uint8_t *pcm, int size) override;
    void ReleaseRender() override;

};

#endif //VIDEOPLAYER_OPENSL_RENDER_H
