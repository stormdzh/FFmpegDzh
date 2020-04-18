//
// Created by tal on 2020-04-18.
//

#ifndef FFMPEGDZH_WLAUDIO_H
#define FFMPEGDZH_WLAUDIO_H

#include "WlQueue.h"
#include "WlPlayState.h"
#include "pthread.h"
#include "string"

#include "WlCallJava.h"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libswresample/swresample.h>

#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
};


class WlAudio {
public:
    //回调到java
    WlCallJava *callJava = NULL;
//    音频流索引
    int streamIndex = -1;
    AVCodecParameters *codecpar;
    AVCodecContext *avCodecContext;

    WlQueue *queue = NULL;
    WlPlayState *playState = NULL;

    pthread_t playThread;
    AVPacket *avPacket = NULL;
    AVFrame *avFrame = NULL;

    uint8_t *buffer = NULL;

    int data_size = 0;
    //采样率
    int sample_rate = 0;

    //    引擎
    SLObjectItf engineObject = NULL;
    SLEngineItf engineEngine = NULL;
    //    混音器
    SLObjectItf outPutMixObject = NULL;
    SLEnvironmentalReverbItf outEnvironmentalReverbItf = NULL;
    //pcm
    SLObjectItf pcmPlayerObj = NULL;
    SLPlayItf pcmPlayer = NULL;
    //    缓冲队列
    SLAndroidSimpleBufferQueueItf pcmBufferQueue = NULL;

    //音频总时长
    int duration = 0;
    //分值
    AVRational time_base;
    //当前时间
    double now_time = 0;

    double clock=0;

    int last_time=0;


public:

    WlAudio(WlPlayState *playState, int sample_rate, WlCallJava *callJava);

    ~WlAudio();

    void play();

    int resampleAudio();

    void initOpenSLES();

    int getCurrentSampleRateForOpensles(int sample_rate);

    void pause();

    void resume();
};

#endif //FFMPEGDZH_WLAUDIO_H
