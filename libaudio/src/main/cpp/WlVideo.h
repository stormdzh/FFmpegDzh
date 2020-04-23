//
// Created by tal on 2020-04-20.
//

#ifndef FFMPEGDZH_WLVIDEO_H
#define FFMPEGDZH_WLVIDEO_H

#include "WlQueue.h"
#include "WlCallJava.h"

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

public:
    WlVideo(WlPlayState *playState, WlCallJava *wlCallJava);

    ~WlVideo();

    void play();

    void release();
};

#endif //FFMPEGDZH_WLVIDEO_H
