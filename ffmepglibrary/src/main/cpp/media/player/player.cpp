//
// Created by yangpc on 2023/10/1.
//

#include "player.h"
#include "../render/video/native_render.h"


Player::Player(JNIEnv *jniEnv, jstring path, jobject surface) {
    m_v_decoder = new VideoDecoder(jniEnv, path);
    m_v_render = new NativeRender(jniEnv, surface);
    m_v_decoder->SetRender(m_v_render);
}

Player::~Player() {

}

void Player::play() {
    if (m_v_decoder != nullptr){
        m_v_decoder->GoOn();
    }
}

void Player::pause() {
    if (m_v_decoder != nullptr){
        m_v_decoder->Pause();
    }
}