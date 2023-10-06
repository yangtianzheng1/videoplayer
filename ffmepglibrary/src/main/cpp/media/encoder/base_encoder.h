//
// Created by yangpc on 2023/10/5.
//

#ifndef VIDEOPLAYER_BASE_ENCODER_H
#define VIDEOPLAYER_BASE_ENCODER_H


#include "i_encoder.h"
#include <thread>
#include <queue>
#include <mutex>
#include "../muxer/mp4_muxer.h"

extern "C"{
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
#include <libavformat/avformat.h>
#include <libavutil/opt.h>
#include <libavutil/frame.h>

};

class BaseEncoder: public IEncoder{

private:

    const char * TAG = "BaseEncoder";

    // 编码格式 ID
    AVCodecID m_codec_id;

    // 线程依附的JVM环境
    JavaVM *m_jvm_for_thread = nullptr;

    // 线程等待锁变量
    pthread_mutex_t m_mutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_cond_t m_cond = PTHREAD_COND_INITIALIZER;

    // 编码器
    const AVCodec *m_codec = nullptr;

    // 编码上下文
    AVCodecContext *m_codec_ctx = nullptr;

    // 编码数据包
    AVPacket *m_encoded_pkt = nullptr;

    // 写入Mp4的输入流索引
    int m_encode_stream_index = 0;

    // 原数据时间基
    AVRational m_src_time_base;

    // 缓冲队列
    std::queue<OneFrame *> m_src_frames;

    // 操作数据锁
    std::mutex m_frames_lock;

    // 状态回调
    IEncodeStateCb *m_state_cb = nullptr;

    bool Init();

    /**
    * 循环拉去已经编码的数据，直到没有数据或者编码完毕
    * @return true 编码结束；false 编码未完成
    */
    bool DrainEncode();

    /**
     * 编码一帧数据
     * @return 错误信息
     */
    int EncodeOneFrame();

    /**
   * 新建编码线程
   */
    void CreateEncodeThread();

    /**
     * 解码静态方法，给线程调用
     */
    static void Encode(std::shared_ptr<BaseEncoder> that);

    void OpenEncoder();

    /**
 * 循环编码
 */
    void LoopEncode();

    void DoRelease();

    void Wait(int second = 0) {
        pthread_mutex_lock(&m_mutex);
        pthread_cond_wait(&m_cond, &m_mutex);
        pthread_mutex_unlock(&m_mutex);
    }

    void SendSignal() {
        pthread_mutex_lock(&m_mutex);
        pthread_cond_signal(&m_cond);
        pthread_mutex_unlock(&m_mutex);
    }

protected:
    // Mp4 封装器
    Mp4Muxer *m_muxer = nullptr;

    virtual void InitContext(AVCodecContext *codec_ctx) = 0;

    virtual int ConfigureMuxerStream(Mp4Muxer *muxer, AVCodecContext *ctx) = 0;

    virtual AVFrame* DealFrame(OneFrame *one_frame) = 0;

    virtual void Release() = 0;

    /**
     * Log前缀
     */
    virtual const char *const LogSpec() = 0;

public:
    BaseEncoder(JNIEnv *env, Mp4Muxer *muxer, AVCodecID codec_id);

    void PushFrame(OneFrame *one_frame) override ;

    bool TooMuchData() override {
        return m_src_frames.size() > 100;
    }

    void SetStateReceiver(IEncodeStateCb *cb) override {
        this->m_state_cb = cb;
    }

};


#endif //VIDEOPLAYER_BASE_ENCODER_H
