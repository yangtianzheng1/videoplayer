//
// Created by 杨天正 on 2022/9/16.
//
#include <unistd.h>
#include "DeMuxer.h"

void DeMuxer::init(DecoderType decoderType) {
    m_AVFormatContext = avformat_alloc_context();
    m_VideoPlayer = new VideoPlayer();
    m_VideoPlayer->init();
//    m_VideoPlayer->SetAVSyncCallback(m_AudioDecoder,
//                                      AudioDecoder::GetVideoDecoderTimestampForAVSync);
}

void DeMuxer::unInit() {
    stop();
    if (decoderThread){
        decoderThread->join();
        delete decoderThread;
        decoderThread = nullptr;
    }
    if (m_VideoPlayer) {
        delete m_VideoPlayer;
        m_VideoPlayer = nullptr;
    }
    if (m_AVFormatContext != nullptr) {
        avformat_close_input(&m_AVFormatContext);
        avformat_free_context(m_AVFormatContext);
        m_AVFormatContext = nullptr;
    }
}

void DeMuxer::start(const char *url, ANativeWindow* window, int width, int height) {
    unique_lock<mutex> lock(m_Mutex);
    m_DecoderState = STATE_DECODING;
    m_Cond.notify_all();

    int result = -1;
    strcpy(m_Url, url);

    do {
        //2.打开文件
        if (avformat_open_input(&m_AVFormatContext, m_Url, nullptr, nullptr) != 0) {
            break;
        }
        //3.获取音视频流信息
        if (avformat_find_stream_info(m_AVFormatContext, nullptr) < 0) {
            break;
        }

        int m_audio_streamIndex = -1;
        int m_video_streamIndex = -1;

        //4.获取音视频流索引
        for (int i = 0; i < m_AVFormatContext->nb_streams; i++) {
            if (m_AVFormatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
                m_audio_streamIndex = i;

            } else if (m_AVFormatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
                m_video_streamIndex = i;
            }
        }

        m_Duration = m_AVFormatContext->duration / AV_TIME_BASE * 1000;//us to ms

//        m_AudioDecoder->setStreamIdx(m_audio_streamIndex);
        m_VideoPlayer->setStreamIdx(m_video_streamIndex);

//        m_AudioDecoder->start(m_AVFormatContext);
        m_VideoPlayer->start(m_AVFormatContext, window, width, height);

    } while (false);

    if (decoderThread) {
        decoderThread->join();
        delete decoderThread;
        decoderThread = nullptr;
    }
    decoderThread = new thread(doAVDecoding, this);
}

void DeMuxer::stop() {
    unique_lock<mutex> lock(m_Mutex);
    m_DecoderState = STATE_STOP;
    m_Cond.notify_all();
    if (decoderThread){
        decoderThread->join();
        delete decoderThread;
        decoderThread = nullptr;
    }
    m_VideoPlayer->stop();
//    m_AudioDecoder->stop();
}

void DeMuxer::pause() {
    unique_lock<mutex> lock(m_Mutex);
    m_DecoderState = STATE_PAUSE;
    m_VideoPlayer->pause();
//    m_AudioDecoder->pause();
}

void DeMuxer::resume() {
    unique_lock<mutex> lock(m_Mutex);
    m_DecoderState = STATE_DECODING;
    m_Cond.notify_all();
    m_VideoPlayer->resume();
//    m_AudioDecoder->resume();
}


void DeMuxer::seekToPosition(float position) {
    unique_lock<mutex> lock(m_Mutex);
    m_SeekPosition = position;
    m_DecoderState = STATE_DECODING;
    m_Cond.notify_all();
}

long DeMuxer::getCurrentPosition() {
//    return m_AudioDecoder->getCurrentPosition();
    return 0;
}

void DeMuxer::setMessageCallback(void *context, MessageCallback callback) {
    m_VideoPlayer->setMessageCallback(context, callback);
//    m_AudioDecoder->setMessageCallback(context, callback);
}

void DeMuxer::doAVDecoding(DeMuxer *deMuxer) {
    deMuxer->decodingLoop();
}

void DeMuxer::decodingLoop() {
    {
        unique_lock<mutex> lock(m_Mutex);
        m_DecoderState = STATE_DECODING;
        lock.unlock();
    }
    while (m_DecoderState != STATE_STOP){
        while (m_DecoderState == STATE_PAUSE) {
            unique_lock<mutex> lock(m_Mutex);
            m_Cond.wait_for(lock, std::chrono::milliseconds(10));
        }
        if (m_DecoderState == STATE_STOP) {
            break;
        }
        if (decodeOnePacket() != 0) {
            unique_lock<mutex> lock(m_Mutex);
            m_DecoderState = STATE_FINISH;
            m_Cond.notify_all();
            break;
        }
    }
}

int DeMuxer::decodeOnePacket() {
    if (m_SeekPosition > 0){
        int64_t seek_target = static_cast<int64_t>(m_SeekPosition * 1000000);//微秒
        int64_t seek_min = INT64_MIN;
        int64_t seek_max = INT64_MAX;
        int seek_ret = avformat_seek_file(m_AVFormatContext, -1, seek_min, seek_target, seek_max,
                                          0);
        if (seek_ret < 0) {
            m_SeekSuccess = false;
        } else {
            m_VideoPlayer->clearCache();
//            m_AudioDecoder->clearCache();
            m_SeekSuccess = true;
            m_SeekPosition = -1;
        }
    }

    AVPacket avPacket = {0};
    int result = av_read_frame(m_AVFormatContext, &avPacket);
    if (result >= 0){
        int buffersizeVideo = m_VideoPlayer->getBufferSize();
//        int buffersizeAudio = m_AudioDecoder->getBufferSize();
        while ((buffersizeVideo > 5) && m_DecoderState == STATE_DECODING &&
               m_SeekPosition < 0) {
            buffersizeVideo = m_VideoPlayer->getBufferSize();
//            buffersizeAudio = m_AudioDecoder->getBufferSize();
            usleep(5 * 1000);
        }
        if (avPacket.stream_index == m_VideoPlayer->getStreamIdx()){
            m_VideoPlayer->pushAVPacket(&avPacket);
        } else{
            av_packet_unref(&avPacket);
        }
        return 0;
    } else{
        return -1;
    }
}
