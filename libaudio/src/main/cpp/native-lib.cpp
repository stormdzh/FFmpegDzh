//
// Created by tal on 2020-04-15.
//
#include <jni.h>
#include <string>
#include <android/log.h>
#include <unistd.h>
#include "AndroidLog.h"

#include "pthread.h"

#include "WlCallJava.h"
#include "WlFFmpeg.h"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
}


WlPlayState *playState = NULL;
bool nativeExit = true;

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

    LOGD("pthread_create result:%d", result);

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



//-----------------------生产者开始------------------------------------

#include "queue"
#include "unistd.h"

pthread_t produc;
pthread_t custom;
pthread_mutex_t mutex;
pthread_cond_t cond;

std::queue<int> queue;


void *producCallback(void *data) {

    while (1) {
        pthread_mutex_lock(&mutex);

        queue.push(1);
        LOGD("生产者生产一个产品，通知消费者消费， 产品数量为 %d", queue.size());
        pthread_cond_signal(&cond);
        pthread_mutex_unlock(&mutex);
        sleep(5);
    }


    pthread_exit(&produc);
}

void *customCallback(void *data) {
    while (1) {
        pthread_mutex_lock(&mutex);

        if (queue.size() > 0) {
            queue.pop();
            LOGD("消费者消费产品，产品数量还剩余 %d ", queue.size());
        } else {
            LOGD("没有产品可以消费， 等待中...");
            pthread_cond_wait(&cond, &mutex);
        }
        pthread_mutex_unlock(&mutex);
        usleep(500 * 1000);
    }
    pthread_exit(&custom);
}


extern "C"
JNIEXPORT void JNICALL
Java_com_stormdzh_libaudio_util_TestJni_mutexThread(JNIEnv *env, jobject instance) {


    for (int i = 0; i < 10; i++) {
        queue.push(1);
    }
    // TODO
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);

    pthread_create(&produc, NULL, producCallback, NULL);
    pthread_create(&custom, NULL, customCallback, NULL);


}

#include "JavaListener.h"

JavaVM *jvm;

JavaListener *javaListener;

pthread_t chidlThread;


void *childCallback(void *data) {
    JavaListener *javaListener1 = (JavaListener *) data;

    javaListener1->onError(0, 101, "c++ call java meid from child thread!");
    pthread_exit(&chidlThread);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_stormdzh_libaudio_util_TestJni_callbackFromC(JNIEnv *env, jobject instance) {

    // TODO
    javaListener = new JavaListener(jvm, env, env->NewGlobalRef(instance));
    //javaListener->onError(1, 100, "c++ call java meid from main thread!");
    pthread_create(&chidlThread, NULL, childCallback, javaListener);


}


JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved) {
    LOGE("JNI_OnLoad");
    JNIEnv *env;
    jvm = vm;
    if (vm->GetEnv((void **) &env, JNI_VERSION_1_6) != JNI_OK) {
        return -1;
    }
    return JNI_VERSION_1_6;
}

//-----------------------生产者结束------------------------------------


#include "TestSLES.h"

extern "C"
JNIEXPORT void JNICALL
Java_com_stormdzh_libaudio_util_TestJni_playPcm(JNIEnv *env, jobject thiz, jstring _pcmpath) {
    // TODO: implement playPcm()
    const char *source = env->GetStringUTFChars(_pcmpath, 0);
    LOGE("播放pcm地址：%s", source);

    TestSLES *testSles = new TestSLES(source);
}


//-----------------------音乐播放器------------------------------------

WlFFmpeg *mFFmpeg = NULL;
WlCallJava *callJava = NULL;

pthread_t thread_start = NULL;

extern "C"
JNIEXPORT void JNICALL
Java_com_stormdzh_libaudio_util_TestJni_prepare(JNIEnv *env, jobject thiz, jstring _source) {
    const char *source = env->GetStringUTFChars(_source, 0);

    LOGE("播放地址：%s", source);

    if (mFFmpeg == NULL) {

        if (callJava == NULL) {
            callJava = new WlCallJava(jvm, env, &thiz);
        }

        callJava->onCallLoad(MAIN_THREAD, true);
        playState = new WlPlayState();
        mFFmpeg = new WlFFmpeg(playState, callJava, source);

        mFFmpeg->prepare();
    }


//    env->ReleaseStringUTFChars(_source, source);
}


void *startCallback(void *data) {
    WlFFmpeg *fFmpeg = (WlFFmpeg *) data;
    if (fFmpeg != NULL) {
        fFmpeg->start();
    }
    pthread_exit(&thread_start);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_stormdzh_libaudio_util_TestJni_start(JNIEnv *env, jobject thiz) {
    LOGE("播放地址 start()");
    if (mFFmpeg != NULL) {
        pthread_create(&thread_start, NULL, startCallback, mFFmpeg);
//        mFFmpeg->start();
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_stormdzh_libaudio_util_TestJni_pause(JNIEnv *env, jobject thiz) {
    // TODO: implement pause()

    if (mFFmpeg != NULL) {
        mFFmpeg->pause();
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_stormdzh_libaudio_util_TestJni_resume(JNIEnv *env, jobject thiz) {
    // TODO: implement resume()
    if (mFFmpeg != NULL) {
        mFFmpeg->resume();
    }
}


extern "C"
JNIEXPORT void JNICALL
Java_com_stormdzh_libaudio_util_TestJni_nstop(JNIEnv *env, jobject thiz) {
    // TODO: implement nstop()
    if (!nativeExit) {

        return;
    }
    nativeExit = false;
    if (mFFmpeg != NULL) {
        mFFmpeg->release();
        delete (mFFmpeg);
        mFFmpeg = NULL;

        if (callJava != NULL) {
            delete (callJava);
            callJava = NULL;
        }

        if (playState != NULL) {
            delete (playState);
            playState = NULL;
        }
    }
    nativeExit = true;

}extern "C"
JNIEXPORT void JNICALL
Java_com_stormdzh_libaudio_util_TestJni_seekto(JNIEnv *env, jobject thiz, jint msc) {
    // TODO: implement seekto()
    if (mFFmpeg != NULL) {
        mFFmpeg->seek(msc);
    }
}extern "C"
JNIEXPORT void JNICALL
Java_com_stormdzh_libaudio_util_TestJni_setVolume(JNIEnv *env, jobject thiz, jint percent) {
    // TODO: implement setVolume()
    if (mFFmpeg != NULL) {
        mFFmpeg->setVolume(percent);
    }
}

//-----------------------音乐播放器------------------------------------