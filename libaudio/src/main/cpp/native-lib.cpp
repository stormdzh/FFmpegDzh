//
// Created by tal on 2020-04-15.
//
#include <jni.h>
#include <string>
#include <android/log.h>
#include <unistd.h>
#include "AndroidLog.h"

#include "pthread.h"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
}




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

//-----------------------测试线程开始------------------------------------
pthread_t thread;
int threadRuning = 1;

void *normalCallBack(void *data) {
    while (threadRuning > 0 && thread != NULL) {
        LOGD("create normal thread from C++!");
        sleep(2);
    }
    //一定要加ruturn 不然报错：A/libc: Fatal signal 4 (SIGILL), code 1, fault addr 0xe63a3456 in tid 8875
    return nullptr;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_stormdzh_libaudio_util_TestJni_normalThread(JNIEnv *env, jobject thiz) {
    LOGD("创建线程");
    threadRuning = 1;
    int result = pthread_create(&thread, NULL, normalCallBack, NULL);

    LOGD("pthread_create result:%d",result);

}

extern "C"
JNIEXPORT void JNICALL
Java_com_stormdzh_libaudio_util_TestJni_stopNormalThread(JNIEnv *env, jobject thiz) {
    LOGD("停止线程");
    threadRuning = 0;
    if (thread != NULL) {
        pthread_detach(thread);
        pthread_exit(&thread);
    }
}

//-----------------------测试线程结束------------------------------------