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

public:
    WlCallJava(JavaVM *javaVm, JNIEnv *jniEnv, jobject *jobj);

    ~WlCallJava();

    void onCallPrepare(int type);

    void onCallLoad(int type, bool load);

    void onCallTimeInfo(int type, int curTime,int duration);


};


#endif //FFMPEGDZH_WLCALLJAVA_H
