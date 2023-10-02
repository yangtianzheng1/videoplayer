//
// Created by 杨天正 on 2022/9/13.
//

#include "OpenSLRender.h"
#include <android/log.h>
#define TAG "OpenSlRender" // 这个是自定义的LOG的标识
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG,TAG ,__VA_ARGS__) // 定义LOGD类型
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,TAG ,__VA_ARGS__) // 定义LOGI类型
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN,TAG ,__VA_ARGS__) // 定义LOGW类型
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,TAG ,__VA_ARGS__) // 定义LOGE类型
#define LOGF(...) __android_log_print(ANDROID_LOG_FATAL,TAG ,__VA_ARGS__) // 定义LOGF类型

void OpenSLRender::init() {
    LOGD("OpenSlRender::init");
    int result = -1;
    do{
        result = createEngine();
        if(result != SL_RESULT_SUCCESS){
            LOGE("OpenSLRender::Init CreateEngine fail. result=%d", result);
            break;
        }
        result = CreateOutputMixer();
        if (result != SL_RESULT_SUCCESS){
            LOGE("OpenSLRender::Init CreateOutputMixer fail. result=%d", result);
            break;
        }
        result = CreateAudioPlayer();
        if (result != SL_RESULT_SUCCESS){
            LOGE("OpenSLRender::Init CreateAudioPlayer fail. result=%d", result);
            break;
        }
        (*m_AudioPlayerPlay)->SetPlayState(m_AudioPlayerPlay, SL_PLAYSTATE_PLAYING);
//        AudioPlayerCallback(m_BufferQueue, this);
    } while (false);

    if(result != SL_RESULT_SUCCESS) {
        LOGE("OpenSLRender::Init fail. result=%d", result);
        release();
    }
}

int OpenSLRender::createEngine() {
    SLresult result = SL_RESULT_SUCCESS;
    do{
        result = slCreateEngine(&m_EngineObj, 0, nullptr,
                                0, nullptr, nullptr);
        if(result != SL_RESULT_SUCCESS){
            LOGE("OpenSLRender::CreateEngine slCreateEngine fail. result=%d", result);
            break;
        }
        result = (*m_EngineObj)->Realize(m_EngineObj, SL_BOOLEAN_FALSE);
        if(result != SL_RESULT_SUCCESS){
            LOGE("OpenSLRender::CreateEngine Realize fail. result=%d", result);
            break;
        }
        result = (*m_EngineObj)->GetInterface(m_EngineObj, SL_IID_ENGINE, &m_EngineEngine);
        if(result != SL_RESULT_SUCCESS){
            LOGE("OpenSLRender::CreateEngine GetInterface fail. result=%d", result);
            break;
        }
    } while (false);
    return result;
}

int OpenSLRender::CreateOutputMixer() {
    SLresult result = SL_RESULT_SUCCESS;
    do {
        const SLInterfaceID mids[1] = {SL_IID_ENVIRONMENTALREVERB};
        const SLboolean mreq[1] = {SL_BOOLEAN_FALSE};
        result = (*m_EngineEngine)->CreateOutputMix(m_EngineEngine, &m_OutputMixObj, 1, mids, mreq);
        if(result != SL_RESULT_SUCCESS){
            LOGE("OpenSLRender::CreateOutputMixer CreateOutputMix fail. result=%d", result);
            break;
        }
        result = (*m_OutputMixObj)->Realize(m_OutputMixObj, SL_BOOLEAN_FALSE);
        if(result != SL_RESULT_SUCCESS){
            LOGE("OpenSLRender::CreateOutputMixer CreateOutputMix fail. result=%d", result);
            break;
        }
    } while (false);
    return result;
}

int OpenSLRender::CreateAudioPlayer() {
    SLDataLocator_AndroidSimpleBufferQueue android_queue = {SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE,
                                                            2};
    SLDataFormat_PCM pcm = {
            SL_DATAFORMAT_PCM,//format type
            (SLuint32)2,//channel count
            SL_SAMPLINGRATE_44_1,//44100hz
            SL_PCMSAMPLEFORMAT_FIXED_16,// bits per sample
            SL_PCMSAMPLEFORMAT_FIXED_16,// container size
            SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT,// channel mask
            SL_BYTEORDER_LITTLEENDIAN // endianness
    };
    SLDataSource slDataSource = {&android_queue, &pcm};

    SLDataLocator_OutputMix outputMix = {SL_DATALOCATOR_OUTPUTMIX, m_OutputMixObj};
    SLDataSink slDataSink = {&outputMix, nullptr};

    const SLInterfaceID ids[3] = {SL_IID_BUFFERQUEUE, SL_IID_EFFECTSEND, SL_IID_VOLUME};
    const SLboolean req[3] = {SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE};

    SLresult result;
    do{
        result = (*m_EngineEngine)->CreateAudioPlayer(m_EngineEngine, &m_AudioPlayerObj, &slDataSource, &slDataSink, 3, ids, req);
        if(result != SL_RESULT_SUCCESS){
            LOGE("OpenSLRender::CreateAudioPlayer CreateAudioPlayer fail. result=%d", result);
            break;
        }
        result = (*m_AudioPlayerObj)->Realize(m_AudioPlayerObj, SL_BOOLEAN_FALSE);
        if(result != SL_RESULT_SUCCESS){
            LOGE("OpenSLRender::CreateAudioPlayer Realize fail. result=%d", result);
            break;
        }
        result = (*m_AudioPlayerObj)->GetInterface(m_AudioPlayerObj, SL_IID_PLAY, &m_AudioPlayerPlay);
        if(result != SL_RESULT_SUCCESS){
            LOGE("OpenSLRender::CreateAudioPlayer GetInterface fail. result=%d", result);
            break;
        }
        result = (*m_AudioPlayerObj)->GetInterface(m_AudioPlayerObj, SL_IID_BUFFERQUEUE, &m_BufferQueue);
        if(result != SL_RESULT_SUCCESS){
            LOGE("OpenSLRender::CreateAudioPlayer GetInterface fail. result=%d", result);
            break;
        }
        result = (*m_BufferQueue)->RegisterCallback(m_BufferQueue, AudioPlayerCallback, this);
        if(result != SL_RESULT_SUCCESS){
            LOGE("OpenSLRender::CreateAudioPlayer RegisterCallback fail. result=%d", result);
            break;
        }
        result = (*m_AudioPlayerObj)->GetInterface(m_AudioPlayerObj, SL_IID_VOLUME, &m_AudioPlayerVolume);
        if(result != SL_RESULT_SUCCESS){
            LOGE("OpenSLRender::CreateAudioPlayer GetInterface fail. result=%d", result);
            break;
        }
    } while (false);

    return result;
}

void OpenSLRender::AudioPlayerCallback(SLAndroidSimpleBufferQueueItf bufferQueue, void *context) {
    OpenSLRender *openSlRender = static_cast<OpenSLRender *>(context);
    openSlRender->HandleAudioFrameQueue();
}

void OpenSLRender::HandleAudioFrameQueue() {
    LOGE("OpenSLRender::HandleAudioFrameQueue QueueSize=%zu", m_AudioFrameQueue.size());
    if (m_AudioPlayerPlay == nullptr) return;
    AudioFrame *audioFrame = m_AudioFrameQueue.front();
    if (nullptr != audioFrame && m_AudioPlayerPlay){
        SLresult result = (*m_BufferQueue)->Enqueue(m_BufferQueue, audioFrame->data, (SLuint32) audioFrame->dataSize);
        if (result == SL_RESULT_SUCCESS){
            m_AudioFrameQueue.pop();
            delete audioFrame;
        }
    }
}

void OpenSLRender::RenderAudioFrame(uint8_t *pData, int dataSize) {
    LOGE("OpenSLRender::RenderAudioFrame pData=%p, dataSize=%d", pData, dataSize);
    if(m_AudioPlayerPlay) {
        if (pData != nullptr && dataSize > 0) {
            //temp resolution, when queue size is too big.
            auto *audioFrame = new AudioFrame(pData, dataSize);
            m_AudioFrameQueue.push(audioFrame);
            HandleAudioFrameQueue();
        }
    }
}

void OpenSLRender::release() {

}

int OpenSLRender::GetAudioFrameQueueSize() {
    return m_AudioFrameQueue.size();
}
