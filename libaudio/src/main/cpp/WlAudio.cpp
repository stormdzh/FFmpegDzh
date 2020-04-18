//
// Created by tal on 2020-04-18.
//

#include "WlAudio.h"

WlAudio::WlAudio(WlPlayState *playState) {

    this->playState = playState;
    queue = new WlQueue(playState);
    buffer = static_cast<uint8_t *>(malloc(44100 * 2 * 2));

}


void *decodePlay(void *data) {

    WlAudio *wlaudio = static_cast<WlAudio *>(data);
    wlaudio->resampleAudio();
    pthread_exit(&wlaudio->playThread);
}


void WlAudio::play() {

    pthread_create(&playThread, NULL, decodePlay, this);

}

int WlAudio::resampleAudio() {

    while (playState != NULL && !playState->exit) {

        avPacket = av_packet_alloc();

        if (queue->getAvPacket(avPacket) != 0) {

            av_packet_free(&avPacket);
            av_free(avPacket);
            avPacket = NULL;
            continue;
        }

        int code_avcodec_send_packet = avcodec_send_packet(avCodecContext, avPacket);
        if (code_avcodec_send_packet != 0) {
            av_packet_free(&avPacket);
            av_free(avPacket);
            avPacket = NULL;
            continue;
        }

        avFrame = av_frame_alloc();
        int code_avcodec_receive_frame = avcodec_receive_frame(avCodecContext, avFrame);

        if (code_avcodec_receive_frame == 0) {
            //重采样
            if (avFrame->channels > 0 && avFrame->channel_layout == 0) {

                avFrame->channel_layout = av_get_default_channel_layout(avFrame->channels);
            } else if (avFrame->channels == 0 && avFrame->channel_layout > 0) {
                avFrame->channels = av_get_channel_layout_nb_channels(avFrame->channel_layout);
            }

            SwrContext *swrContext = NULL;
            swrContext = swr_alloc_set_opts(
                    NULL,
                    AV_CH_LAYOUT_STEREO,
                    AV_SAMPLE_FMT_S16,
                    avFrame->sample_rate,
                    avFrame->channel_layout,
                    static_cast<AVSampleFormat>(avFrame->format),
                    avFrame->sample_rate,
                    NULL, NULL
            );

            if (!swrContext || swr_init(swrContext) < 0) {

                av_packet_free(&avPacket);
                av_free(avPacket);
                avPacket = NULL;

                av_frame_free(&avFrame);
                av_free(avFrame);
                avFrame = NULL;

                if (swrContext != NULL) {
                    swr_free(&swrContext);
                    avFrame = NULL;
                }

                continue;

            }

            int nb = swr_convert(
                    swrContext,
                    &buffer,
                    avFrame->nb_samples,
                    (const uint8_t **)avFrame->data,
                    avFrame->nb_samples);


            int out_channels = av_get_channel_layout_nb_channels(AV_CH_LAYOUT_STEREO);
            data_size = nb * out_channels * av_get_bytes_per_sample(AV_SAMPLE_FMT_S16);
            LOGE("data size:%d", data_size);


            av_packet_free(&avPacket);
            av_free(avPacket);
            avPacket = NULL;

            av_frame_free(&avFrame);
            av_free(avFrame);
            avFrame = NULL;

            swr_free(&swrContext);
            swrContext = NULL;


        } else {
            av_packet_free(&avPacket);
            av_free(avPacket);
            avPacket = NULL;

            av_frame_free(&avFrame);
            av_free(avFrame);
            avFrame = NULL;

        }


    }
    return 0;
}
