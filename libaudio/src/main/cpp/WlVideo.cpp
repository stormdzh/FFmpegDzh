//
// Created by tal on 2020-04-20.
//

#include "WlVideo.h"

WlVideo::WlVideo(WlPlayState *playState, WlCallJava *wlCallJava) {

    this->playState = playState;
    this->wlCallJava = wlCallJava;
    queue = new WlQueue(playState);
}

WlVideo::~WlVideo() {

}

void *playVide(void *data) {
    WlVideo *video = (WlVideo *) data;

    while (video->playState != NULL && !video->playState->exit) {

        AVPacket *avPacket = av_packet_alloc();

        if(video->queue->getAvPacket(avPacket)==0){
          LOGE("线程中获取视频AVPacket");
        }

        av_packet_free(&avPacket);
        av_free(avPacket);
        avPacket = NULL;
    }


    pthread_exit(&video->thread_play);
}

void WlVideo::play() {

    pthread_create(&thread_play, NULL, playVide, this);
}
