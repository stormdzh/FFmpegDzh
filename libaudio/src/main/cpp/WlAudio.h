//
// Created by tal on 2020-04-18.
//

#ifndef FFMPEGDZH_WLAUDIO_H
#define FFMPEGDZH_WLAUDIO_H

#include "WlQueue.h"
#include "WlPlayState.h"

extern "C" {
#include <libavcodec/avcodec.h>
};


class WlAudio {
public:
    int streamIndex = -1;
    AVCodecParameters *codecpar;
    AVCodecContext *avCodecContext;

    WlQueue *queue = NULL;
    WlPlayState *playState = NULL;

public:

    WlAudio(WlPlayState *playState);

    ~WlAudio();
};

#endif //FFMPEGDZH_WLAUDIO_H
