//
// Created by yangpc on 2023/10/5.
//

#ifndef VIDEOPLAYER_I_ENCODER_H
#define VIDEOPLAYER_I_ENCODER_H

#include "../one_frame.h"
#include "i_encode_state_cb.h"

class IEncoder{
public:
    virtual void PushFrame(OneFrame* oneFrame ) = 0;
    virtual bool TooMuchData() = 0;
    virtual void SetStateReceiver(IEncodeStateCb *cb) = 0;

};
#endif //VIDEOPLAYER_I_ENCODER_H
