//
// Created by tal on 2020-04-18.
//
#include "WlQueue.h"

WlQueue::WlQueue(WlPlayState *state) {

    this->state = state;
    pthread_mutex_init(&mutexPacket, NULL);
    pthread_cond_init(&countPacket, NULL);

}

WlQueue::~WlQueue() {

    pthread_mutex_destroy(&mutexPacket);
    pthread_cond_destroy(&countPacket);
}

//把AVPacket添加到队列
int WlQueue::putAvPacket(AVPacket *packet) {

    pthread_mutex_lock(&mutexPacket);
    queuePacket.push(packet);
    LOGE("添加了解码数据到队列：个数为：%d", queuePacket.size());
    pthread_cond_signal(&countPacket);
    pthread_mutex_unlock(&mutexPacket);
    return 0;
}

//从队列中获取AVPacket
int WlQueue::getAvPacket(AVPacket *packet) {
    pthread_mutex_lock(&mutexPacket);
    while (state != NULL && !state->exit) {

        if (queuePacket.size() > 0) {
            AVPacket *&avPacket = queuePacket.front();
            if (av_packet_ref(packet, avPacket) == 0) {
                queuePacket.pop();
            }
            av_packet_free(&avPacket);
            av_free(avPacket);
            avPacket = NULL;
            LOGE("从队列里面拿一个avPacket，还剩下：%d", queuePacket.size());
            break;
        } else {
            pthread_cond_wait(&countPacket, &mutexPacket);
        }
    }
    pthread_mutex_unlock(&mutexPacket);
    return 0;
}

int WlQueue::getQueueSize() {
    int size=0;
    pthread_mutex_lock(&mutexPacket);
    size=queuePacket.size();
    pthread_mutex_unlock(&mutexPacket);
    return size;
}


