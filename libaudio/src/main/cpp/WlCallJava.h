//
// Created by tal on 2020-04-17.
//

#ifndef FFMPEGDZH_WLCALLJAVA_H
#define FFMPEGDZH_WLCALLJAVA_H


#include <jni.h>
#include <cwchar>

#define MAIN_THREAD 0
#define CHILD_THREAD 1

class WlCallJava {

public:
    JavaVM *javaVm = NULL;
    JNIEnv *jniEnv = NULL;
    jobject jobj;
    jmethodID jmid_prepare;
    jmethodID jmid_load;
    jmethodID jmid_timeinfo;
    jmethodID jmid_error;
    jmethodID jmid_complete;
    jmethodID jmid_pcmtoaac;
    jmethodID jmid_pcminfo;
    jmethodID jmid_renderyuv;
    jmethodID jmid_supportMediaCodec;
    jmethodID jmid_initMediaCodec;
    jmethodID jmid_decodeavpackt;

public:
    WlCallJava(JavaVM *javaVm, JNIEnv *jniEnv, jobject *jobj);

    ~WlCallJava();

    void onCallPrepare(int type);

    void onCallLoad(int type, bool load);

    void onCallTimeInfo(int type, int curTime,int duration);

    void onCallError(int type, int code,char *msg);

    void onCallComplete(int type);

    void onCallPcmToAAc(int type,int size,void *buffer);

    void onCallPcmInfo(int type,void *buffer,int size);

    void onCallRenderYUV(int width,int height,uint8_t *fy,uint8_t *fu,uint8_t *fv);

    bool onCallIsSupportMediaCode(const char* ffcodecname);

    void onCallInitMediaCodec(const char* mine,int width,int heght,int csd0_size,uint8_t* csd_0,int csd1_size,uint8_t* csd_1);

    void onCallDecodeAvpackt(int datasize,uint8_t* avpacktdata);


};


#endif //FFMPEGDZH_WLCALLJAVA_H
