//
// Created by yangpc on 2023/10/5.
//

#ifndef VIDEOPLAYER_I_ENCODE_STATE_CB_H
#define VIDEOPLAYER_I_ENCODE_STATE_CB_H

#include "../one_frame.h"
class IEncodeStateCb{
public:
    virtual void EncodeStart() = 0;
    virtual void EncodeSend() = 0;
    virtual void EncodeFrame(void *data) = 0;
    virtual void EncodeProgress(long time) = 0;
    virtual void EncodeFinish() = 0;
};

#endif //VIDEOPLAYER_I_ENCODE_STATE_CB_H
