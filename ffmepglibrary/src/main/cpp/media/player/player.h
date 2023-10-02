//
// Created by yangpc on 2023/10/1.
//

#ifndef VIDEOPLAYER_PLAYER_H
#define VIDEOPLAYER_PLAYER_H


#include "../decoder/v_decoder.h"

class Player {

private:
    VideoDecoder *m_v_decoder;
    VideoRender *m_v_render;

public:

    Player(JNIEnv *jniEnv, jstring path, jobject surface);

    ~Player();

    void play();
    void pause();

};


#endif //VIDEOPLAYER_PLAYER_H
