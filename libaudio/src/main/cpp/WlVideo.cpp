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

        if (video->playState->load) {
            av_usleep(1000 * 100);
            continue;
        }

        if (video->playState->isSeeking) {
            av_usleep(1000 * 100);
            continue;
        }

        if (video->queue->getQueueSize() == 0) {
            if (!video->playState->load) {
                video->playState->load = true;
                video->wlCallJava->onCallLoad(CHILD_THREAD, true);
            }
            av_usleep(1000 * 100);
            continue;
        } else { //有值就可以解码了

            if (video->playState->load) {
                video->playState->load = false;
                video->wlCallJava->onCallLoad(CHILD_THREAD, false);
            }
        }

        AVPacket *avPacket = av_packet_alloc();

        if (video->queue->getAvPacket(avPacket) != 0) {
            LOGE("没有拿到AVPacket");
            av_packet_free(&avPacket);
            av_free(avPacket);
            avPacket = NULL;
            av_usleep(1000 * 100);
            continue;
        }

        if (avcodec_send_packet(video->avCodecContext, avPacket) != 0) {
            av_packet_free(&avPacket);
            av_free(avPacket);
            avPacket = NULL;
            continue;
        }

        AVFrame *avFrame = av_frame_alloc();

        if (avcodec_receive_frame(video->avCodecContext, avFrame) != 0) {

            av_frame_free(&avFrame);
            av_free(avFrame);
            avFrame = NULL;

            av_packet_free(&avPacket);
            av_free(avPacket);
            avPacket = NULL;
            av_usleep(1000 * 100);
            continue;
        }

        LOGE("子线程解码到avframe成功");


        av_frame_free(&avFrame);
        av_free(avFrame);
        avFrame = NULL;

        av_packet_free(&avPacket);
        av_free(avPacket);
        avPacket = NULL;

    }


    pthread_exit(&video->thread_play);
}

void WlVideo::play() {

    pthread_create(&thread_play, NULL, playVide, this);
}

void WlVideo::release() {

    if (queue != NULL) {
        delete (queue);
        queue = NULL;
    }

    if (avCodecContext != NULL) {
        avcodec_close(avCodecContext);
        avcodec_free_context(&avCodecContext);
        avCodecContext = NULL;
    }


    if (playState != NULL) {
        playState = NULL;
    }

    if (wlCallJava != NULL) {
        wlCallJava = NULL;
    }


}
