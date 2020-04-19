//
// Created by tal on 2020-04-18.
//

#ifndef FFMPEGDZH_WLFFMPEG_H
#define FFMPEGDZH_WLFFMPEG_H

#include "AndroidLog.h"
#include <android/log.h>
#include "pthread.h"
#include "WlCallJava.h"
#include "WlAudio.h"

extern "C"
{
#include "libavformat/avformat.h"
#include <libavutil/time.h>
};

class WlFFmpeg {
public:
    WlCallJava *callJava = NULL;
    const char *url = NULL;
    pthread_t decodeThread;
    AVFormatContext *pFormatCtx = NULL;
    WlAudio *audio = NULL;
    WlPlayState *playState = NULL;
    //初始化的锁
    pthread_mutex_t init_mutex;
    //seek的锁
    pthread_mutex_t seek_mutex;

    bool initexit = false;

    int duration = 0;

public:

    WlFFmpeg(WlPlayState *playState, WlCallJava *callJava, const char *url);

    ~WlFFmpeg();

    void prepare();

    void decodeFFmpegThread();

    void start();


    void pause();

    void resume();

    void release();

    //seek 单位是秒
    void seek(int64_t secds);

    void setVolume(int percent);

    void setMute(int type);
};

#endif //FFMPEGDZH_WLFFMPEG_H
