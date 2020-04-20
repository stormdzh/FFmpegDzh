//
// Created by tal on 2020-04-20.
//

#ifndef FFMPEGDZH_WLVIDEO_H
#define FFMPEGDZH_WLVIDEO_H

#include "WlQueue.h"
#include "WlCallJava.h"

extern "C" {
#include <libavcodec/avcodec.h>
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
};

#endif //FFMPEGDZH_WLVIDEO_H