//
// Created by tal on 2020-04-18.
//

#include "WlAudio.h"

WlAudio::WlAudio(WlPlayState *playState, int sample_rate) {

    this->sample_rate = sample_rate;
    this->playState = playState;
    queue = new WlQueue(playState);
    buffer = static_cast<uint8_t *>(malloc(44100 * 2 * 2));

}


void *decodePlay(void *data) {

    WlAudio *wlaudio = static_cast<WlAudio *>(data);
//    wlaudio->resampleAudio();
    wlaudio->initOpenSLES();
    pthread_exit(&wlaudio->playThread);
}

//FILE *outFile = fopen("/storage/emulated/0/resample.pcm", "w");

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
                    (const uint8_t **) avFrame->data,
                    avFrame->nb_samples);


            int out_channels = av_get_channel_layout_nb_channels(AV_CH_LAYOUT_STEREO);
            data_size = nb * out_channels * av_get_bytes_per_sample(AV_SAMPLE_FMT_S16);
            LOGE("解码数据 data size:%d", data_size);


//            fwrite(buffer, 1, data_size, outFile);


            av_packet_free(&avPacket);
            av_free(avPacket);
            avPacket = NULL;

            av_frame_free(&avFrame);
            av_free(avFrame);
            avFrame = NULL;

            swr_free(&swrContext);
            swrContext = NULL;
            break;

        } else {
            av_packet_free(&avPacket);
            av_free(avPacket);
            avPacket = NULL;

            av_frame_free(&avFrame);
            av_free(avFrame);
            avFrame = NULL;
            continue;

        }


    }
    return data_size;
}

void mPcmBufferCallBack(SLAndroidSimpleBufferQueueItf bf, void *context) {

    WlAudio *wlaudio = static_cast<WlAudio *>(context);
    if (wlaudio != NULL) {
        int bufferSize = wlaudio->resampleAudio();
        if (bufferSize > 0) {
            (*wlaudio->pcmBufferQueue)->Enqueue(wlaudio->pcmBufferQueue, wlaudio->buffer,
                                                bufferSize);
        }
    }
}

void WlAudio::initOpenSLES() {

    //-------------------------------------------
    slCreateEngine(&engineObject, 0, 0, 0, 0, 0);
    (*engineObject)->Realize(engineObject, SL_BOOLEAN_FALSE);
    (*engineObject)->GetInterface(engineObject, SL_IID_ENGINE, &engineEngine);


    //-------------------------------------------
    const SLInterfaceID mids[1] = {SL_IID_ENVIRONMENTALREVERB};
    const SLboolean mreq[1] = {SL_BOOLEAN_FALSE};
    (*engineEngine)->CreateOutputMix(engineEngine, &outPutMixObject, 1, mids, mreq);
    (*outPutMixObject)->Realize(outPutMixObject, SL_BOOLEAN_FALSE);
    (*outPutMixObject)->GetInterface(outPutMixObject, SL_IID_ENVIRONMENTALREVERB,
                                     &outEnvironmentalReverbItf);
    //设置属性
    SLEnvironmentalReverbSettings reverbSettings = SL_I3DL2_ENVIRONMENT_PRESET_STONECORRIDOR;
    (*outEnvironmentalReverbItf)->SetEnvironmentalReverbProperties(outEnvironmentalReverbItf,
                                                                   &reverbSettings);

    //-------------------------------------------
    //创建播放器
    SLDataLocator_OutputMix outPutMix = {SL_DATALOCATOR_OUTPUTMIX, outPutMixObject};
    SLDataSink slDataSink = {&outPutMix, NULL};

    SLDataLocator_AndroidBufferQueue android_queue = {SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE, 2};
    SLDataFormat_PCM pcm = {
            SL_DATAFORMAT_PCM,
            2,
//            SL_SAMPLINGRATE_44_1,
            (SLuint32)getCurrentSampleRateForOpensles(sample_rate),
            SL_PCMSAMPLEFORMAT_FIXED_16,
            SL_PCMSAMPLEFORMAT_FIXED_16,
            SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT,
            SL_BYTEORDER_LITTLEENDIAN
    };


    const SLInterfaceID ids[1] = {SL_IID_BUFFERQUEUE};
    const SLboolean req[1] = {SL_BOOLEAN_TRUE};

//    const SLInterfaceID ids[3] = {SL_IID_BUFFERQUEUE, SL_IID_EFFECTSEND, SL_IID_VOLUME};
//    const SLboolean req[3] = {SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE};

    SLDataSource slDataSource = {&android_queue, &pcm};
    (*engineEngine)->CreateAudioPlayer(engineEngine, &pcmPlayerObj, &slDataSource, &slDataSink, 1,
                                       ids, req);


    (*pcmPlayerObj)->Realize(pcmPlayerObj, SL_BOOLEAN_FALSE);
    (*pcmPlayerObj)->GetInterface(pcmPlayerObj, SL_IID_PLAY, &pcmPlayer);

    //-------------------------------------------
    //设置缓冲
    (*pcmPlayerObj)->GetInterface(pcmPlayerObj, SL_IID_BUFFERQUEUE, &pcmBufferQueue);
    (*pcmBufferQueue)->RegisterCallback(pcmBufferQueue, mPcmBufferCallBack, this);
    (*pcmPlayer)->SetPlayState(pcmPlayer, SL_PLAYSTATE_PLAYING);

    mPcmBufferCallBack(pcmBufferQueue, this);

}

int WlAudio::getCurrentSampleRateForOpensles(int sample_rate) {
    int rate = 0;
    switch (sample_rate) {
        case 8000:
            rate = SL_SAMPLINGRATE_8;
            break;
        case 11025:
            rate = SL_SAMPLINGRATE_11_025;
            break;
        case 12000:
            rate = SL_SAMPLINGRATE_12;
            break;
        case 16000:
            rate = SL_SAMPLINGRATE_16;
            break;
        case 22050:
            rate = SL_SAMPLINGRATE_22_05;
            break;
        case 24000:
            rate = SL_SAMPLINGRATE_24;
            break;
        case 32000:
            rate = SL_SAMPLINGRATE_32;
            break;
        case 44100:
            rate = SL_SAMPLINGRATE_44_1;
            break;
        case 48000:
            rate = SL_SAMPLINGRATE_48;
            break;
        case 64000:
            rate = SL_SAMPLINGRATE_64;
            break;
        case 88200:
            rate = SL_SAMPLINGRATE_88_2;
            break;
        case 96000:
            rate = SL_SAMPLINGRATE_96;
            break;
        case 192000:
            rate = SL_SAMPLINGRATE_192;
            break;
        default:
            rate = SL_SAMPLINGRATE_44_1;
    }
    return rate;
}
