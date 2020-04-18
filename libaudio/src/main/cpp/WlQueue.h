//
// Created by tal on 2020-04-18.
//

#ifndef FFMPEGDZH_WLQUEUE_H
#define FFMPEGDZH_WLQUEUE_H

#include <pthread.h>
#include "queue"
#include "unistd.h"
#include "AndroidLog.h"
#include <android/log.h>
#include "WlPlayState.h"

extern "C" {
#include <libavcodec/avcodec.h>
};

class WlQueue {
public:
    std::queue<AVPacket *> queuePacket;
    pthread_mutex_t mutexPacket;
    pthread_cond_t countPacket;
    WlPlayState *state =NULL;

public:
    WlQueue(WlPlayState *state);

    ~WlQueue();

    int putAvPacket(AVPacket *packet);
    int getAvPacket(AVPacket *packet);

    int getQueueSize();

    //释放资源
    void clearAvpacket();
};


#endif //FFMPEGDZH_WLQUEUE_H