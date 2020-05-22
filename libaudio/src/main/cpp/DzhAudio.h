//
// Created by dzh on 2020-04-18.
//

#ifndef FFMPEGDZH_DZHAUDIO_H
#define FFMPEGDZH_DZHAUDIO_H

#include "DzhQueue.h"
#include "DzhPlayState.h"
#include "pthread.h"
#include "string"

#include "DzgCallJava.h"

#include "SoundTouch.h"

using namespace soundtouch;

#include "DzhPcmBean.h"
#include "DzhBufferQueue.h"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libswresample/swresample.h>
#include <libavutil/time.h>

#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
};


class DzhAudio {
public:
    //回调到java
    WlCallJava *callJava = NULL;
//    音频流索引
    int streamIndex = -1;
    AVCodecParameters *codecpar;
    AVCodecContext *avCodecContext;

    DzhQueue *queue = NULL;
    DzhPlayState *playState = NULL;

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

    //package 中有多个frame
    bool readFrameFinised=true;

    bool  isCut = false;
    int endTime =0;
    bool showPcm=false;

    //pcm数据分包
    pthread_t pcmCallbackThread=NULL;
    DzhBufferQueue *bufferQueue;
    int defaultPcmSize=4096;

    //seek线程锁
    pthread_mutex_t codecMutex;



public:

    DzhAudio(DzhPlayState *playState, int sample_rate, WlCallJava *callJava);

    ~DzhAudio();

    //播放
    void play();

    //音频重采样
    int resampleAudio(void **pcmBuf);

    //初始化 opensles
    void initOpenSLES();

    //获取采样率
    int getCurrentSampleRateForOpensles(int sample_rate);

    //暂停
    void pause();

    //恢复播放
    void resume();

    //停止
    void stop();

    //释放
    void release();

    //设置音量
    void setVolume(int percent);

    //设置声道 0:右声道 1：左声道  2：立体声
    void setMute(int type);


    //sountouch 里面获取数据
    int getSoundTouchData();

    //变调
    void setPitch(double newPitch);

    //变速
    void setTempo(double newTempo);


    //获取音频数据的振幅
    int getPCMDB(char *pcmdata, size_t pcmsize);


};

#endif //FFMPEGDZH_DZHAUDIO_H
