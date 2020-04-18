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
};

class WlFFmpeg {
public:
    int testnum = -1;
    WlCallJava *callJava = NULL;
    const char *url = NULL;
    pthread_t decodeThread;
    AVFormatContext *pFormatCtx = NULL;
    WlAudio *audio = NULL;
    WlPlayState *playState = NULL;
public:

    WlFFmpeg(WlPlayState *playState, WlCallJava *callJava, const char *url);

    ~WlFFmpeg();

    void prepare();

    void decodeFFmpegThread();

    void start();
};

#endif //FFMPEGDZH_WLFFMPEG_H
