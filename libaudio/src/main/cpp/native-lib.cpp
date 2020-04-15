//
// Created by tal on 2020-04-15.
//
#include <jni.h>
#include <string>
#include <android/log.h>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
}


#define TAG "MFFMPEG"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__)

extern "C"
JNIEXPORT jstring JNICALL Java_com_stormdzh_libaudio_util_TestJni_stringFromJni(JNIEnv *env,
                                                                                jobject) {

    std::string hello = "hello jni";

    return env->NewStringUTF(hello.c_str());
}


extern "C"
JNIEXPORT jstring JNICALL Java_com_stormdzh_libaudio_util_TestJni_testFFmpeg(JNIEnv *env, jobject) {

    LOGI("测试FFmpeg开始");
    av_register_all();
    AVCodec *c_temp = av_codec_next(NULL);
    while (c_temp != NULL) {
        switch (c_temp->type) {
            case AVMEDIA_TYPE_VIDEO:
                LOGI("[Video]:%s", c_temp->name);
                break;
            case AVMEDIA_TYPE_AUDIO:
                LOGI("[Audio]:%s", c_temp->name);
                break;
            default:
                LOGI("[Other]:%s", c_temp->name);
                break;
        }
        c_temp = c_temp->next;
    }
    LOGI("测试FFmpeg结束");
    unsigned int i = avcodec_version();
    const char *versionInfo = av_version_info();
    LOGI("avcodec_version:%s", versionInfo);
    return env->NewStringUTF(versionInfo);
}
