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
    jmid_pcminfo = jniEnv->GetMethodID(jlz, "onCallPcmInfo", "([BI)V");
    jmid_renderyuv = jniEnv->GetMethodID(jlz, "onCallRenderYUV", "(II[B[B[B)V");
    jmid_supportMediaCodec = jniEnv->GetMethodID(jlz, "onCallIsSupportMediaCode",
                                                 "(Ljava/lang/String;)Z");
    jmid_initMediaCodec = jniEnv->GetMethodID(jlz, "initMediaCodec", "(Ljava/lang/String;II[B[B)V");
    jmid_decodeavpackt = jniEnv->GetMethodID(jlz, "decodeAvPackt", "(I[B)V");
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

    if (type == MAIN_THREAD) {
        jbyteArray jbuffer = jniEnv->NewByteArray(size);
        jniEnv->SetByteArrayRegion(jbuffer, 0, size, static_cast<const jbyte *>(buffer));

        jniEnv->CallVoidMethod(jobj, jmid_pcmtoaac, size, jbuffer);

        jniEnv->DeleteLocalRef(jbuffer);

    } else if (type == CHILD_THREAD) {
        JNIEnv *jniEnv;
        if (javaVm->AttachCurrentThread(&jniEnv, 0) != JNI_OK) {
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

void WlCallJava::onCallPcmInfo(int type, void *buffer, int size) {
    if (type == MAIN_THREAD) {
        jbyteArray jbuffer = jniEnv->NewByteArray(size);
        jniEnv->SetByteArrayRegion(jbuffer, 0, size, static_cast<const jbyte *>(buffer));

        jniEnv->CallVoidMethod(jobj, jmid_pcminfo, jbuffer, size);

        jniEnv->DeleteLocalRef(jbuffer);

    } else if (type == CHILD_THREAD) {
        JNIEnv *jniEnv;
        if (javaVm->AttachCurrentThread(&jniEnv, 0) != JNI_OK) {
            LOGE("call onCallComplete worng");
            return;
        }
        jbyteArray jbuffer = jniEnv->NewByteArray(size);
        jniEnv->SetByteArrayRegion(jbuffer, 0, size, static_cast<const jbyte *>(buffer));

        jniEnv->CallVoidMethod(jobj, jmid_pcminfo, jbuffer, size);

        jniEnv->DeleteLocalRef(jbuffer);

        javaVm->DetachCurrentThread();
    }
}

void WlCallJava::onCallRenderYUV(int width, int height, uint8_t *fy, uint8_t *fu, uint8_t *fv) {

    JNIEnv *jniEnv;
    if (javaVm->AttachCurrentThread(&jniEnv, 0) != JNI_OK) {
        LOGE("call onCallComplete worng");
        return;
    }

    jbyteArray y = jniEnv->NewByteArray(width * height);
    jniEnv->SetByteArrayRegion(y, 0, width * height, reinterpret_cast<const jbyte *>(fy));

    jbyteArray u = jniEnv->NewByteArray(width * height / 4);
    jniEnv->SetByteArrayRegion(u, 0, width * height / 4, reinterpret_cast<const jbyte *>(fu));

    jbyteArray v = jniEnv->NewByteArray(width * height / 4);
    jniEnv->SetByteArrayRegion(v, 0, width * height / 4, reinterpret_cast<const jbyte *>(fv));


    jniEnv->CallVoidMethod(jobj, jmid_renderyuv, width, height, y, u, v);

    jniEnv->DeleteLocalRef(y);
    jniEnv->DeleteLocalRef(u);
    jniEnv->DeleteLocalRef(v);

    javaVm->DetachCurrentThread();

}

bool WlCallJava::onCallIsSupportMediaCode(const char *ffcodecname) {

    JNIEnv *jniEnv;
    if (javaVm->AttachCurrentThread(&jniEnv, 0) != JNI_OK) {
        LOGE("call onCallComplete worng");
        return false;
    }

    bool support = false;

    jstring codecName = jniEnv->NewStringUTF(ffcodecname);
    support = jniEnv->CallBooleanMethod(jobj, jmid_supportMediaCodec, codecName);
    jniEnv->DeleteLocalRef(codecName);
    javaVm->DetachCurrentThread();
    return support;
}

void WlCallJava::onCallInitMediaCodec(const char* mine,int width, int height, int csd0_size,uint8_t* csd_0,int csd1_size, uint8_t* csd_1) {
    JNIEnv *jniEnv;
    if (javaVm->AttachCurrentThread(&jniEnv, 0) != JNI_OK) {
        LOGE("call onCallComplete worng");
        return ;
    }

    jstring type=jniEnv->NewStringUTF(mine);

    jbyteArray csd0 = jniEnv->NewByteArray(csd0_size);
    jniEnv->SetByteArrayRegion(csd0, 0, csd0_size, reinterpret_cast<const jbyte *>(csd_0));

    jbyteArray csd1 = jniEnv->NewByteArray(csd1_size);
    jniEnv->SetByteArrayRegion(csd1, 0, csd0_size, reinterpret_cast<const jbyte *>(csd_1));

    jniEnv->CallVoidMethod(jobj, jmid_initMediaCodec, type,width, height, csd0, csd1);

    jniEnv->DeleteLocalRef(type);
    jniEnv->DeleteLocalRef(csd0);
    jniEnv->DeleteLocalRef(csd1);
    javaVm->DetachCurrentThread();

}

void WlCallJava::onCallDecodeAvpackt(int datasize, uint8_t *packtdata) {

    JNIEnv *jniEnv;
    if (javaVm->AttachCurrentThread(&jniEnv, 0) != JNI_OK) {
        LOGE("call onCallComplete worng");
        return ;
    }

    jbyteArray data = jniEnv->NewByteArray(datasize);
    jniEnv->SetByteArrayRegion(data, 0, datasize, reinterpret_cast<const jbyte *>(packtdata));

    jniEnv->CallVoidMethod(jobj, jmid_decodeavpackt, datasize,data);

    jniEnv->DeleteLocalRef(data);
    javaVm->DetachCurrentThread();
}

