//
// Created by tal on 2020-04-18.
//

#ifndef FFMPEGDZH_WLAUDIO_H
#define FFMPEGDZH_WLAUDIO_H

#include "WlQueue.h"
#include "WlPlayState.h"
#include "pthread.h"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libswresample/swresample.h>
};


class WlAudio {
public:
    int streamIndex = -1;
    AVCodecParameters *codecpar;
    AVCodecContext *avCodecContext;

    WlQueue *queue = NULL;
    WlPlayState *playState = NULL;

    pthread_t playThread;
    AVPacket *avPacket = NULL;
    AVFrame *avFrame = NULL;

    uint8_t *buffer = NULL;

    int data_size=0;

public:

    WlAudio(WlPlayState *playState);

    ~WlAudio();

    void play();

    int resampleAudio();
};

#endif //FFMPEGDZH_WLAUDIO_H
