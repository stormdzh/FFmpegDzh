//
// Created by dzh on 2017-12-3.
//

#ifndef WLPLAYER_BUFFERQUEUE_H
#define WLPLAYER_BUFFERQUEUE_H

#include "deque"
#include "DzhPlayState.h"
#include "DzhPcmBean.h"
#include <android/log.h>

extern "C"
{
#include <libavcodec/avcodec.h>
#include "pthread.h"
};

class DzhBufferQueue {

public:
    std::deque<DzhPcmBean *> queueBuffer;
    pthread_mutex_t mutexBuffer;
    pthread_cond_t condBuffer;
    DzhPlayState *wlPlayStatus = NULL;

public:
    DzhBufferQueue(DzhPlayState *playStatus);

    ~DzhBufferQueue();

    int putBuffer(SAMPLETYPE *buffer, int size);

    int getBuffer(DzhPcmBean **pcmBean);

    int clearBuffer();

    void release();

    int getBufferSize();

    int noticeThread();
};


#endif //WLPLAYER_BUFFERQUEUE_H
