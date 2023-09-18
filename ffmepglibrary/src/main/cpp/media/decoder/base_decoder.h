//
// Created by 杨天正 on 2023/9/17.
//

#ifndef VIDEOPLAYER_BASE_DECODER_H
#define VIDEOPLAYER_BASE_DECODER_H

#include <jni.h>
#include <string>
#include <unistd.h>
#include <thread>
#include <stdio.h>
#include "i_decoder.h"
#include "decode_state.h"
#include "../../utils/logger.h"
#include "i_decode_state_cb.h"

extern "C" {
#include "../../include/libavformat/avformat.h"
#include "../../include/libavcodec/avcodec.h"
#include "../../include/libavutil/frame.h"
};

class BaseDecoder : public IDecoder {

private:

    const char *TAG = "BaseDecoder";

    // 解码信息上下文
    AVFormatContext *m_format_ctx = nullptr;

    const AVCodec *m_codec = nullptr;

    AVCodecContext *m_codec_ctx = nullptr;

    AVPacket *m_packet = nullptr;

    AVFrame *m_frame = nullptr;

    int64_t m_cur_t_s = 0;

    long m_duration = 0;

    // 开始播放的时间
    int64_t m_started_t = -1;

    // 解码状态
    DecodeState m_state = STOP;

    // 数据流索引
    int m_stream_index = -1;

    //线程依赖的 JVM 环境
    JavaVM *m_jvm_for_thread = nullptr;

    // 原始路径 jstring 引用，否则无法在线程中操作
    jobject m_path_ref = nullptr;

    //经过转换的路径
    const char *m_path = nullptr;

    pthread_mutex_t m_mutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_cond_t m_cond = PTHREAD_COND_INITIALIZER;

    bool m_for_synthesizer = false;


    /**
    * 初始化
    * @param env jvm环境
    * @param path 本地文件路径
    */
    void Init(JNIEnv *env, jstring path);

    /**
    * 初始化FFMpeg相关的参数
    * @param env jvm环境
    */
    void InitFFMpegDecoder(JNIEnv * env);

    // 新建解码线程
    void CreateDecodeThread();

    /**
    * 分配解码过程中需要的缓存
    */
    void AllocFrameBuffer();

    //静态解码方法 用于解码线程回调
    static void Decode(std::shared_ptr<BaseDecoder> that);

    //循环解码
    void LoopDecode();

    /**
    * 获取当前帧时间戳
    */
    void ObtainTimeStamp();

    /**
    * 解码完成
    * @param env jvm环境
    */
    void DoneDecode(JNIEnv *env);

    /**
    * 时间同步
    */
    void SyncRender();


protected:

    IDecodeStateCb *m_state_cb = nullptr;

    // 进入等待
    void Wait(long second = 0);

    //恢复解码
    void SendSignal();

    /**
    * 子类准备回调方法
    * @note 注：在解码线程中回调
    * @param env 解码线程绑定的JVM环境
    */
    virtual void Prepare(JNIEnv* env) = 0;

    /**
    * 子类渲染回调方法
    * @note 注：在解码线程中回调
    * @param frame 视频：一帧YUV数据；音频：一帧PCM数据
    */
    virtual void Render(AVFrame *frame) = 0;

    /**
   * 子类释放资源回调方法
   */
    virtual void Release() = 0;

    /**
   * Log前缀
   */
    virtual const char *const LogSpec() = 0;

    void CallbackState(DecodeState status);

    /**
    * 音视频索引
    */
    virtual AVMediaType GetMediaType() = 0;

    /**
     * 是否需要自动循环解码
     */
    virtual bool NeedLoopDecode() = 0;

    /**
    * 解码一帧数据
    * @return
    */
    AVFrame* DecodeOneFrame();

    /**
    * 是否为合成器提供解码
    * @return true 为合成器提供解码 false 解码播放
    */
    bool ForSynthesizer() {
        return m_for_synthesizer;
    }

public:

    BaseDecoder(JNIEnv *env, jstring path);

    virtual ~BaseDecoder();

    void GoOn() override;

    void Pause() override;

    void Stop() override;

    bool IsRunning() override;

    long GetDuration() override;

    long GetCurPos() override;

    int64_t GetCurMsTime() {
        struct timeval tv{};
        gettimeofday(&tv, nullptr);
        int64_t ts = (int64_t)tv.tv_sec * 1000 + tv.tv_usec / 1000;
        return ts;
    }

};


#endif //VIDEOPLAYER_BASE_DECODER_H
