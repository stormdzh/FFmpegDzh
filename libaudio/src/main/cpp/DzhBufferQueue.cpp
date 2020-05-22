//
// Created by dzh on 2017-12-3.
// pcm数据分包
//

#include "DzhBufferQueue.h"
#include "AndroidLog.h"

DzhBufferQueue::DzhBufferQueue(DzhPlayState *playStatus) {
    wlPlayStatus = playStatus;
    pthread_mutex_init(&mutexBuffer, NULL);
    pthread_cond_init(&condBuffer, NULL);
}

DzhBufferQueue::~DzhBufferQueue() {
    wlPlayStatus = NULL;
    pthread_mutex_destroy(&mutexBuffer);
    pthread_cond_destroy(&condBuffer);
    if(LOG_DEBUG)
    {
        LOGE("DzhBufferQueue 释放完了");
    }
}

void DzhBufferQueue::release() {

    if(LOG_DEBUG)
    {
        LOGE("DzhBufferQueue::release");
    }
    noticeThread();
    clearBuffer();

    if(LOG_DEBUG)
    {
        LOGE("DzhBufferQueue::release success");
    }
}

int DzhBufferQueue::putBuffer(SAMPLETYPE *buffer, int size) {
    pthread_mutex_lock(&mutexBuffer);
    DzhPcmBean *pcmBean = new DzhPcmBean(buffer, size);
    queueBuffer.push_back(pcmBean);
    pthread_cond_signal(&condBuffer);
    pthread_mutex_unlock(&mutexBuffer);
    return 0;
}

int DzhBufferQueue::getBuffer(DzhPcmBean **pcmBean) {

    pthread_mutex_lock(&mutexBuffer);

    while(wlPlayStatus != NULL && !wlPlayStatus->exit)
    {
        if(queueBuffer.size() > 0)
        {
            *pcmBean = queueBuffer.front();
            queueBuffer.pop_front();
            break;
        } else{
            if(!wlPlayStatus->exit)
            {
                pthread_cond_wait(&condBuffer, &mutexBuffer);
            }
        }
    }
    pthread_mutex_unlock(&mutexBuffer);
    return 0;
}

int DzhBufferQueue::clearBuffer() {

    pthread_cond_signal(&condBuffer);
    pthread_mutex_lock(&mutexBuffer);
    while (!queueBuffer.empty())
    {
        DzhPcmBean *pcmBean = queueBuffer.front();
        queueBuffer.pop_front();
        delete(pcmBean);
    }
    pthread_mutex_unlock(&mutexBuffer);
    return 0;
}

int DzhBufferQueue::getBufferSize() {
    int size = 0;
    pthread_mutex_lock(&mutexBuffer);
    size = queueBuffer.size();
    pthread_mutex_unlock(&mutexBuffer);
    return size;
}


int DzhBufferQueue::noticeThread() {
    pthread_cond_signal(&condBuffer);
    return 0;
}

