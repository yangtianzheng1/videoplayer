//
// Created by 杨天正 on 2022/9/10.
//

#include <jni.h>
#include <string>
#include <unistd.h>

extern "C" {
#include "include/libavcodec/avcodec.h"
#include "include/libavformat/avformat.h"
#include "include/libavcodec/jni.h"
}

#define JNI_FUNC(retType, bindClass, name)  JNIEXPORT retType JNICALL Java_com_example_ffmepglibrary_##bindClass##_##name
#define JNI_ARGS    JNIEnv *env, jobject thiz

extern "C" {
 JNI_FUNC(jstring, FFmepgHelper, getFFmpegVersion)(JNI_ARGS) {
     const char* version = av_version_info();
     return env->NewStringUTF(version);
 }

JNI_FUNC(jstring, FFmepgHelper, getFFmpegConfiguration)(JNI_ARGS) {
    const char* config = avcodec_configuration();
    return env->NewStringUTF(config);
}
}