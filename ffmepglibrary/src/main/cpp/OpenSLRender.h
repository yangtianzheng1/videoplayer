//
// Created by 杨天正 on 2022/9/13.
//

#ifndef VIDEOPLAYER_OPENSLRENDER_H
#define VIDEOPLAYER_OPENSLRENDER_H

#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
#include <queue>
#include "AudioFrame.h"
#include <thread>

#define MAX_QUEUE_BUFFER_SIZE 5

using namespace std;

class OpenSLRender {
public:
    OpenSLRender(){};
    ~OpenSLRender(){};

    void init();
    void unInit();
    void ClearAudioCache();
    int createEngine();
    int CreateOutputMixer();
    int CreateAudioPlayer();
    void HandleAudioFrameQueue();
    void RenderAudioFrame(uint8_t *pData, int dataSize);
    static void AudioPlayerCallback(SLAndroidSimpleBufferQueueItf bufferQueue, void *context);

private:

    int GetAudioFrameQueueSize();
    void StartRender();

    static void CreateSLWaitingThread(OpenSLRender *openSlRender);

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

    queue<AudioFrame *> m_AudioFrameQueue;
    thread *m_thread = nullptr;
    mutex   m_Mutex;
    condition_variable m_Cond;
    volatile bool m_Exit = false;

};


#endif //VIDEOPLAYER_OPENSLRENDER_H
