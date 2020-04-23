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

        if (avFrame->format = AV_PIX_FMT_YUV420P) {
            //是yuv420p  渲染
            //回调数据
            LOGE("当前视频是yuv420p------");
            video->wlCallJava->onCallRenderYUV(video->avCodecContext->width,video->avCodecContext->height,
                    avFrame->data[0],avFrame->data[1],avFrame->data[2]);
        } else {
            //转换成yuv420p
            AVFrame *pFrameYUV420P = av_frame_alloc();
            int num = av_image_get_buffer_size(AV_PIX_FMT_YUV420P,
                                               video->avCodecContext->width,
                                               video->avCodecContext->height, 1);

            uint8_t *buffer = static_cast<uint8_t *>(av_malloc(num * sizeof(uint8_t)));
            //填充数据
            av_image_fill_arrays(pFrameYUV420P->data,
                                 pFrameYUV420P->linesize,
                                 buffer,
                                 AV_PIX_FMT_YUV420P,
                                 video->avCodecContext->width,
                                 video->avCodecContext->height,
                                 1);
            SwsContext *sws_ctx = sws_getContext(
                    video->avCodecContext->width,
                    video->avCodecContext->height,
                    video->avCodecContext->pix_fmt,
                    video->avCodecContext->width,
                    video->avCodecContext->height,
                    AV_PIX_FMT_YUV420P,
                    SWS_BICUBIC, NULL, NULL, NULL
            );

            if (!sws_ctx) {
                if (avFrame != NULL) {
                    av_frame_free(&pFrameYUV420P);
                    av_free(pFrameYUV420P);
                    pFrameYUV420P = NULL;
                }

                continue;
            }

            //转换
            sws_scale(sws_ctx,
                      avFrame->data,
                      avFrame->linesize,
                      0,
                      avFrame->height,
                      pFrameYUV420P->data,
                      pFrameYUV420P->linesize
            );

            //回调数据
            video->wlCallJava->onCallRenderYUV(video->avCodecContext->width,video->avCodecContext->height,
                                               pFrameYUV420P->data[0],pFrameYUV420P->data[1],pFrameYUV420P->data[2]);
            LOGE("当前视频 不是yuv420p------");
            //渲染
            av_frame_free(&pFrameYUV420P);
            av_free(pFrameYUV420P);
            pFrameYUV420P = NULL;
            av_free(buffer);
            sws_freeContext(sws_ctx);


        }


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
