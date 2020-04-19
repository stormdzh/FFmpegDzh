//
// Created by tal on 2020-04-17.
//

#include <android/log.h>
#include "WlCallJava.h"
#include "AndroidLog.h"

WlCallJava::WlCallJava(JavaVM *javaVm, JNIEnv *jniEnv, jobject *obj) {
    this->javaVm = javaVm;
    this->jniEnv = jniEnv;
    this->jobj = *obj;
    this->jobj = jniEnv->NewGlobalRef(jobj);

    jclass jlz = jniEnv->GetObjectClass(jobj);
    if (!jlz) {
        LOGI("WlCallJava  jlz 错误");
        return;
    }

    jmid_prepare = jniEnv->GetMethodID(jlz, "onCallPrepare", "()V");
    jmid_load = jniEnv->GetMethodID(jlz, "onCallLoad", "(Z)V");
    jmid_timeinfo = jniEnv->GetMethodID(jlz, "onCallTimeInfo", "(II)V");
    jmid_error = jniEnv->GetMethodID(jlz, "onCallError", "(ILjava/lang/String;)V");
    jmid_complete = jniEnv->GetMethodID(jlz, "onCallComplete", "()V");
    jmid_pcmtoaac = jniEnv->GetMethodID(jlz, "encodecPcmToAAc", "(I[B)V");
}

void WlCallJava::onCallPrepare(int type) {
    if (type == MAIN_THREAD) {

        jniEnv->CallVoidMethod(jobj, jmid_prepare);
    } else if (type == CHILD_THREAD) {

        JNIEnv *jniEnv1;
        if (javaVm->AttachCurrentThread(&jniEnv1, 0) != JNI_OK) {
            LOGI("WlCallJava  AttachCurrentThread 错误");
            return;
        }
        jniEnv1->CallVoidMethod(jobj, jmid_prepare);
        javaVm->DetachCurrentThread();
    }

}

void WlCallJava::onCallLoad(int type, bool load) {
    if (type == MAIN_THREAD) {

        jniEnv->CallVoidMethod(jobj, jmid_load, load);
    } else if (type == CHILD_THREAD) {

        JNIEnv *jniEnv1;
        if (javaVm->AttachCurrentThread(&jniEnv1, 0) != JNI_OK) {
            LOGI("WlCallJava  AttachCurrentThread 错误");
            return;
        }
        jniEnv1->CallVoidMethod(jobj, jmid_load, load);
        javaVm->DetachCurrentThread();
    }
}

void WlCallJava::onCallTimeInfo(int type, int curTime, int duration) {
    if (type == MAIN_THREAD) {

        jniEnv->CallVoidMethod(jobj, jmid_timeinfo, curTime, duration);
    } else if (type == CHILD_THREAD) {

        JNIEnv *jniEnv1;
        if (javaVm->AttachCurrentThread(&jniEnv1, 0) != JNI_OK) {
            LOGI("WlCallJava  AttachCurrentThread 错误");
            return;
        }
        jniEnv1->CallVoidMethod(jobj, jmid_timeinfo, curTime, duration);
        javaVm->DetachCurrentThread();
    }
}

WlCallJava::~WlCallJava() {

}

void WlCallJava::onCallError(int type, int code, char *msg) {

    if (type == MAIN_THREAD) {
        jstring jmsg = jniEnv->NewStringUTF(msg);
        jniEnv->CallVoidMethod(jobj, jmid_error, code, jmsg);
        jniEnv->DeleteLocalRef(jmsg);
    } else if (type == CHILD_THREAD) {

        JNIEnv *jniEnv1;
        if (javaVm->AttachCurrentThread(&jniEnv1, 0) != JNI_OK) {
            LOGI("WlCallJava  AttachCurrentThread 错误");
            return;
        }
        jstring jmsg = jniEnv1->NewStringUTF(msg);
        jniEnv1->CallVoidMethod(jobj, jmid_error, code, jmsg);
        jniEnv1->DeleteLocalRef(jmsg);
        javaVm->DetachCurrentThread();
    }
}

void WlCallJava::onCallComplete(int type) {

    if (type == MAIN_THREAD) {
        jniEnv->CallVoidMethod(jobj, jmid_complete);
    } else if (type == CHILD_THREAD) {

        JNIEnv *jniEnv1;
        if (javaVm->AttachCurrentThread(&jniEnv1, 0) != JNI_OK) {
            LOGI("WlCallJava  AttachCurrentThread 错误");
            return;
        }
        jniEnv1->CallVoidMethod(jobj, jmid_complete);
        javaVm->DetachCurrentThread();
    }
}

void WlCallJava::onCallPcmToAAc(int type, int size, void *buffer) {

    if(type == MAIN_THREAD)
    {
        jbyteArray jbuffer = jniEnv->NewByteArray(size);
        jniEnv->SetByteArrayRegion(jbuffer, 0, size, static_cast<const jbyte *>(buffer));

        jniEnv->CallVoidMethod(jobj, jmid_pcmtoaac, size, jbuffer);

        jniEnv->DeleteLocalRef(jbuffer);

    }
    else if(type == CHILD_THREAD)
    {
        JNIEnv *jniEnv;
        if(javaVm->AttachCurrentThread(&jniEnv, 0) != JNI_OK)
        {
                LOGE("call onCallComplete worng");
            return;
        }
        jbyteArray jbuffer = jniEnv->NewByteArray(size);
        jniEnv->SetByteArrayRegion(jbuffer, 0, size, static_cast<const jbyte *>(buffer));

        jniEnv->CallVoidMethod(jobj, jmid_pcmtoaac, size, jbuffer);

        jniEnv->DeleteLocalRef(jbuffer);

        javaVm->DetachCurrentThread();
    }
}

