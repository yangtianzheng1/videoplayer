//
// Created by 杨天正 on 2022/9/13.
//

#ifndef VIDEOPLAYER_OPENSLRENDER_H
#define VIDEOPLAYER_OPENSLRENDER_H

#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
#include <queue>
#include "AudioFrame.h"

#define MAX_QUEUE_BUFFER_SIZE 3

class OpenSLRender {
public:
    OpenSLRender(){};
    ~OpenSLRender(){};
    void init();
    int createEngine();
    int CreateOutputMixer();
    int CreateAudioPlayer();
    void HandleAudioFrameQueue();
    void RenderAudioFrame(uint8_t *pData, int dataSize);
    static void AudioPlayerCallback(SLAndroidSimpleBufferQueueItf bufferQueue, void *context);
    void release();

private:

    int GetAudioFrameQueueSize();

    //opensl es
    SLObjectItf m_EngineObj = nullptr;
    SLEngineItf m_EngineEngine = nullptr;

    //混音器
    SLObjectItf m_OutputMixObj = nullptr;

    //播放器
    SLObjectItf m_AudioPlayerObj = nullptr;
    SLPlayItf m_AudioPlayerPlay = nullptr;
    SLAndroidSimpleBufferQueueItf m_BufferQueue = nullptr;
    SLVolumeItf m_AudioPlayerVolume = nullptr;

    std::queue<AudioFrame *> m_AudioFrameQueue;

};


#endif //VIDEOPLAYER_OPENSLRENDER_H
