//
// Created by tal on 2020-04-20.
//

#ifndef FFMPEGDZH_WLVIDEO_H
#define FFMPEGDZH_WLVIDEO_H

#include "WlQueue.h"
#include "WlCallJava.h"
#include "WlAudio.h"


#define CODEC_YUV 0
#define CODEC_MEDIACODEC 1

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavutil/time.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
};

class WlVideo {
public:
    int streamIndex = -1;
    AVCodecContext *avCodecContext = NULL;
    AVCodecParameters *codecpar = NULL;
    WlQueue *queue = NULL;
    WlPlayState *playState = NULL;
    WlCallJava *wlCallJava = NULL;

    AVRational time_base;

    pthread_t thread_play;

    //处理音视频同步
    WlAudio *audio=NULL;
    double clock=0;
    double delayTime=0;
    double defualtDelayTime=0.04;

    //seek线程锁
    pthread_mutex_t codecMutex;

    //解码方式标示 -默认使用软解码
    int codectype=CODEC_YUV;

    AVBSFContext *abs_ctx=NULL;


public:
    WlVideo(WlPlayState *playState, WlCallJava *wlCallJava);

    ~WlVideo();

    void play();

    void release();

    //获取音频进度和视频进度差值
    double getFrameDiffTime(AVFrame *avFrame,AVPacket *avPacket);
    //获取视频需要延迟的时间
    double getDelayTime(double diff);
};

#endif //FFMPEGDZH_WLVIDEO_H
