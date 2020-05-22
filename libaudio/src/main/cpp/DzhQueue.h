//
// Created by dzh on 2020-04-18.
//

#ifndef FFMPEGDZH_DZHQUEUE_H
#define FFMPEGDZH_DZHQUEUE_H

#include <pthread.h>
#include "queue"
#include "unistd.h"
#include "AndroidLog.h"
#include <android/log.h>
#include "DzhPlayState.h"

extern "C" {
#include <libavcodec/avcodec.h>
};

class DzhQueue {
public:
    std::queue<AVPacket *> queuePacket;
    pthread_mutex_t mutexPacket;
    pthread_cond_t countPacket;
    DzhPlayState *state =NULL;

public:
    DzhQueue(DzhPlayState *state);

    ~DzhQueue();

    int putAvPacket(AVPacket *packet);
    int getAvPacket(AVPacket *packet);

    int getQueueSize();

    //释放资源
    void clearAvpacket();

    void notifyQueue();
};


#endif //FFMPEGDZH_DZHQUEUE_H