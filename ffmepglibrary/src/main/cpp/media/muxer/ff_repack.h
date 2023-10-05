//
// Created by yangpc on 2023/10/4.
//

#ifndef VIDEOPLAYER_FF_REPACK_H
#define VIDEOPLAYER_FF_REPACK_H

#include <jni.h>
extern "C"{
#include <libavformat/avformat.h>
};


class FFRepack {
private:
    const char *TAG = "FFRepack";

    AVFormatContext *m_in_format_cxt = nullptr;
    AVFormatContext *m_out_format_cxt = nullptr;

    int OpenSrcFile(const char* srcPath);

    int InitMuxerParams(const char* destPath);

public:
    FFRepack(JNIEnv *env,jstring in_path, jstring out_path);

    void Start();

    void Write(AVPacket avPacket);

    void Release();

};


#endif //VIDEOPLAYER_FF_REPACK_H
