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

