//
// Created by yangpc on 2023/10/6.
//

#ifndef VIDEOPLAYER_I_MUXER_CB_H
#define VIDEOPLAYER_I_MUXER_CB_H

class IMuxerCb{
public:
    virtual void OnMuxFinished() = 0;
};

#endif //VIDEOPLAYER_I_MUXER_CB_H
