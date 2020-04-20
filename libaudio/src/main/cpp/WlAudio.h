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

#include "SoundTouch.h"

using namespace soundtouch;

extern "C" {
#include <libavcodec/avcodec.h>
#include <libswresample/swresample.h>
#include <libavutil/time.h>

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
    //采样个数
    int nb = 0;

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

    //音量控制
    SLVolumeItf pcmVolumePlay = NULL;
    //声道控制
    SLMuteSoloItf pcmMuteSoloPlay = NULL;

    //音频总时长
    int duration = 0;
    //分值
    AVRational time_base;
    //当前时间
    double now_time = 0;

    double clock = 0;

    int last_time = 0;

    //默认音量
    int defaultVolume = 100;


    //soundTouch
    SoundTouch *soundTouch = NULL;
    SAMPLETYPE *sampleBuffer = NULL;
    bool finished = true;
    uint8_t *out_buffer = NULL;
    int sound_touch_nb = 0; //soundtouch返回的采样个数

    //是否录制pcm的开关
    bool is_record_pcm = false;


public:

    WlAudio(WlPlayState *playState, int sample_rate, WlCallJava *callJava);

    ~WlAudio();

    void play();

    int resampleAudio(void **pcmBuf);

    void initOpenSLES();

    int getCurrentSampleRateForOpensles(int sample_rate);

    void pause();

    void resume();

    void stop();

    void release();

    void setVolume(int percent);

    //0:右声道 1：左声道  2：立体声
    void setMute(int type);


    //sountouch
    int getSoundTouchData();

    //变调
    void setPitch(double newPitch);

    //变速
    void setTempo(double newTempo);


    //获取音频数据的振幅
    int getPCMDB(char *pcmdata, size_t pcmsize);


};

#endif //FFMPEGDZH_WLAUDIO_H
