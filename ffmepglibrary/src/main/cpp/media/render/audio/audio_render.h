//
// Created by yangpc on 2023/10/2.
//

#ifndef VIDEOPLAYER_AUDIO_RENDER_H
#define VIDEOPLAYER_AUDIO_RENDER_H

class AudioRender{
public:
    virtual void InitRender() = 0;
    virtual void Render(uint8_t *pcm, int size) = 0;
    virtual void ReleaseRender() = 0;
    virtual ~AudioRender(){};
};

#endif //VIDEOPLAYER_AUDIO_RENDER_H
