//
// Created by 杨天正 on 2022/9/16.
//

#ifndef VIDEOPLAYER_MESSAGECALLBACK_H
#define VIDEOPLAYER_MESSAGECALLBACK_H

enum DecoderType{
    SOFT,
    HW,
};

enum DecoderState {
    STATE_UNKNOWN,
    STATE_DECODING,
    STATE_PAUSE,
    STATE_STOP,
    STATE_FINISH,
};

enum DecoderMsg {
    MSG_DECODER_INIT_ERROR,
    MSG_DECODER_READY,
    MSG_DECODER_DONE,
    MSG_REQUEST_RENDER,
    MSG_DECODING_TIME
};

typedef void (*MessageCallback)(void *, int, float);

typedef long (*AVSyncCallback)(void *);

#endif //VIDEOPLAYER_MESSAGECALLBACK_H
