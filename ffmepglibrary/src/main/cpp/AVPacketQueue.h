//
// Created by 杨天正 on 2022/9/14.
//

#ifndef VIDEOPLAYER_AVPACKETQUEUE_H
#define VIDEOPLAYER_AVPACKETQUEUE_H

#include <thread>

extern "C"{
#include <libavcodec/packet.h>
};

using namespace std;

typedef struct AVPacketNode{
    AVPacket pkt;
    struct AVPacketNode *next;
} AVPacketNode;

class AVPacketQueue {
public:
    AVPacketQueue();
    virtual ~AVPacketQueue();
    int PushPacket(AVPacket *pkt);
    int PushNullPacket(int stream_index);
    void Flush();
    void Abort();
    void Start();

    int GetPacket(AVPacket *pkt);
    int GetPacket(AVPacket *pkt, int block);
    int GetPacketSize();
    int GetSize();
    int64_t GetDuration();
    int IsAbort();

private:
    int Put(AVPacket *pkt);

private:
    mutex m_Mutex;
    condition_variable m_CondVar;
    AVPacketNode *first_pkt;
    AVPacketNode *last_pkt;
    int nb_packets;
    int size;
    int64_t duration;
    volatile int abort_request;
};


#endif //VIDEOPLAYER_AVPACKETQUEUE_H
