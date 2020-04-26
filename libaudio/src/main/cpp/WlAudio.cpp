//
// Created by tal on 2020-04-18.
//

#include "WlAudio.h"

WlAudio::WlAudio(WlPlayState *playState, int sample_rate, WlCallJava *callJava) {

    this->callJava = callJava;
    this->sample_rate = sample_rate;
    this->playState = playState;
    queue = new WlQueue(playState);
    buffer = static_cast<uint8_t *>(malloc(sample_rate * 2 * 2));

    //soundtouch
    sampleBuffer = static_cast<SAMPLETYPE *>(malloc(sample_rate * 2 * 2));
    soundTouch = new SoundTouch();
    soundTouch->setSampleRate(sample_rate);
    soundTouch->setChannels(2);
    //设置变速
    soundTouch->setPitch(1.0f);
    //设置变调
    soundTouch->setTempo(1.0f);

    this->showPcm = false;
    this->endTime = 0;
    this->isCut = false;

    pthread_mutex_init(&codecMutex,NULL);


}


void *decodePlay(void *data) {

    WlAudio *wlaudio = static_cast<WlAudio *>(data);
//    wlaudio->resampleAudio();
    wlaudio->initOpenSLES();
//    pthread_exit(&wlaudio->playThread);
    return 0;
}

void *pcmCallBack(void *data) {
    WlAudio *audio = static_cast<WlAudio *>(data);
    audio->bufferQueue = new WlBufferQueue(audio->playState);

    while (audio->playState != NULL && !audio->playState->exit) {

        WlPcmBean *pcmBean = NULL;
        audio->bufferQueue->getBuffer(&pcmBean);
        if (pcmBean == NULL) {
            continue;
        }
        if (pcmBean->buffsize <= audio->defaultPcmSize) { //不需要分包
            //调用java硬编码吧pcm转为aac
            if (audio->is_record_pcm) {
                audio->callJava->onCallPcmToAAc(CHILD_THREAD, pcmBean->buffsize,
                                                pcmBean->buffer);
            }
            if (audio->showPcm) {
                LOGE("返回pcm数据！");
                audio->callJava->onCallPcmInfo(CHILD_THREAD, pcmBean->buffer,
                                               pcmBean->buffsize);
            }
        } else { //需要分包
            int pack_num = pcmBean->buffsize / audio->defaultPcmSize;
            int pack_sub = pcmBean->buffsize % audio->defaultPcmSize;

            for (int i = 0; i < pack_num; i++) {  //分整数
                char *bf = static_cast<char *>(malloc(audio->defaultPcmSize));
                memcpy(bf, pcmBean->buffer + i * audio->defaultPcmSize, audio->defaultPcmSize);


                //调用java硬编码吧pcm转为aac
                if (audio->is_record_pcm) {
                    audio->callJava->onCallPcmToAAc(CHILD_THREAD, audio->defaultPcmSize,
                                                    bf);
                }
                if (audio->showPcm) {
                    LOGE("返回pcm数据！");
                    audio->callJava->onCallPcmInfo(CHILD_THREAD, bf,
                                                   audio->defaultPcmSize);
                }
                free(bf);
                bf = NULL;
            }

            if (pack_sub > 0) {
                char *bf = static_cast<char *>(malloc(pack_sub));
                memcpy(bf, pcmBean->buffer + pack_num * audio->defaultPcmSize, pack_sub);


                //调用java硬编码吧pcm转为aac
                if (audio->is_record_pcm) {
                    audio->callJava->onCallPcmToAAc(CHILD_THREAD, pack_sub,
                                                    bf);
                }
                if (audio->showPcm) {
                    LOGE("返回pcm数据！");
                    audio->callJava->onCallPcmInfo(CHILD_THREAD, bf,
                                                   pack_sub);
                }
                free(bf);
                bf = NULL;
            }
        }

        if (pcmBean != NULL) {
            delete (pcmBean);
            pcmBean = NULL;
        }
    }

    pthread_exit(&audio->pcmCallbackThread);
//    return 0;

}

//FILE *outFile = fopen("/storage/emulated/0/resample.pcm", "w");

void WlAudio::play() {

    if(playState!=NULL&&!playState->exit) {
        pthread_create(&playThread, NULL, decodePlay, this);
        pthread_create(&pcmCallbackThread, NULL, pcmCallBack, this);
    }


}

//**pcmBuf 参数也是为了soundtouch加的
int WlAudio::resampleAudio(void **pcmBuf) {

    while (playState != NULL && !playState->exit) {

        if (playState->isSeeking) {
            //休眠100毫秒
            av_usleep(1000 * 100);
            continue;
        }

        if (queue->getQueueSize() <= 0) {
            if (!playState->load) {
                playState->load = true;
                callJava->onCallLoad(CHILD_THREAD, true);
            }
            //休眠100毫秒
            av_usleep(1000 * 100);
            continue;
        } else {
            if (playState->load) {
                playState->load = false;
                callJava->onCallLoad(CHILD_THREAD, false);
            }
        }

        if (readFrameFinised) { //只有当前的package中的frame都都去完了，才读取下个package
            avPacket = av_packet_alloc();

            if (queue->getAvPacket(avPacket) != 0) {

                av_packet_free(&avPacket);
                av_free(avPacket);
                avPacket = NULL;
                continue;
            }

            pthread_mutex_lock(&codecMutex);

            int code_avcodec_send_packet = avcodec_send_packet(avCodecContext, avPacket);
            if (code_avcodec_send_packet != 0) {
                av_packet_free(&avPacket);
                av_free(avPacket);
                avPacket = NULL;
                pthread_mutex_unlock(&codecMutex);
                continue;
            }
        }

        avFrame = av_frame_alloc();
        int code_avcodec_receive_frame = avcodec_receive_frame(avCodecContext, avFrame);

        readFrameFinised = false;
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
                readFrameFinised = true;

                pthread_mutex_unlock(&codecMutex);
                continue;

            }

            nb = swr_convert(
                    swrContext,
                    &buffer,
                    avFrame->nb_samples,
                    (const uint8_t **) avFrame->data,
                    avFrame->nb_samples);


            int out_channels = av_get_channel_layout_nb_channels(AV_CH_LAYOUT_STEREO);
            data_size = nb * out_channels * av_get_bytes_per_sample(AV_SAMPLE_FMT_S16);
            LOGE("解码数据 data size:%d", data_size);


//            fwrite(buffer, 1, data_size, outFile);
            now_time = avFrame->pts * av_q2d(time_base);
            if (now_time < clock) {
                now_time = clock;
            }
            clock = now_time;

            //Soundtouch 变速使用
            *pcmBuf = buffer;

//            av_packet_free(&avPacket);
//            av_free(avPacket);
//            avPacket = NULL;

            av_frame_free(&avFrame);
            av_free(avFrame);
            avFrame = NULL;

            swr_free(&swrContext);
            swrContext = NULL;
            pthread_mutex_unlock(&codecMutex);
            break;

        } else {

            readFrameFinised = true;
            av_packet_free(&avPacket);
            av_free(avPacket);
            avPacket = NULL;

            av_frame_free(&avFrame);
            av_free(avFrame);
            avFrame = NULL;
            pthread_mutex_unlock(&codecMutex);
            continue;

        }


    }
    return data_size;
}


int WlAudio::getSoundTouchData() {

    while (playState != NULL && !playState->exit) {
        out_buffer = NULL;
        if (finished) {
            finished = false;
            data_size = resampleAudio(reinterpret_cast<void **>(&out_buffer));
            LOGE("22222222222  data_size:%d",data_size);
            if (data_size > 0) {
                for (int i = 0; i < data_size / 2 + 1; i++) {
                    if(sampleBuffer!=NULL&&out_buffer!=NULL) {
                        sampleBuffer[i] = (out_buffer[i * 2] | ((out_buffer[i * 2 + 1]) << 8));
                    }
                }
                soundTouch->putSamples(sampleBuffer, nb);
                sound_touch_nb = soundTouch->receiveSamples(sampleBuffer, data_size / 4);
            } else {
                soundTouch->flush();
            }
        }
        if (sound_touch_nb == 0) {
            finished = true;
            continue;
        } else {
            if (out_buffer == NULL) {
                sound_touch_nb = soundTouch->receiveSamples(sampleBuffer, data_size / 4);
                if (sound_touch_nb == 0) {
                    finished = true;
                    continue;
                }
            }
            return sound_touch_nb;
        }
    }
    return 0;
}


void mPcmBufferCallBack(SLAndroidSimpleBufferQueueItf bf, void *context) {

    WlAudio *wlaudio = static_cast<WlAudio *>(context);
    if (wlaudio != NULL) {
//        int bufferSize = wlaudio->resampleAudio();
        int bufferSize = wlaudio->getSoundTouchData();
        if (bufferSize > 0) {
            wlaudio->clock += bufferSize / ((double) (wlaudio->sample_rate * 2 * 2));
            //回调进度
            if (wlaudio->clock - wlaudio->last_time > 0.5) {
                wlaudio->last_time = wlaudio->clock;
                wlaudio->callJava->onCallTimeInfo(CHILD_THREAD, wlaudio->clock,
                                                  wlaudio->duration);
            }

            //调用java硬编码吧pcm转为aac
//            if (wlaudio->is_record_pcm) {
//                wlaudio->callJava->onCallPcmToAAc(CHILD_THREAD, bufferSize * 2 * 2,
//                                                  wlaudio->sampleBuffer);
//            }
            wlaudio->bufferQueue->putBuffer(wlaudio->sampleBuffer, bufferSize * 2 * 2);

            //获取音频数据的振幅
            int db = wlaudio->getPCMDB(reinterpret_cast<char *>(wlaudio->sampleBuffer),
                                       bufferSize * 2 * 2);
            LOGE("--------pcm数据的振幅：----》%d", db);

//            (*wlaudio->pcmBufferQueue)->Enqueue(wlaudio->pcmBufferQueue, wlaudio->buffer,bufferSize);
            (*wlaudio->pcmBufferQueue)->Enqueue(wlaudio->pcmBufferQueue, wlaudio->sampleBuffer,
                                                bufferSize * 2 * 2);

            //播放裁剪音频
            if (wlaudio->isCut) {
//                if (wlaudio->showPcm) {
//                    LOGE("返回pcm数据！");
//                    wlaudio->callJava->onCallPcmInfo(CHILD_THREAD, wlaudio->sampleBuffer,
//                                                     bufferSize * 2 * 2);
//                }
                if (wlaudio->clock > wlaudio->endTime) {
                    LOGE("裁剪退出！");
                    wlaudio->playState->exit = true;
                }
            }
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

    SLDataLocator_AndroidBufferQueue android_queue = {SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE,
                                                      2};
    SLDataFormat_PCM pcm = {
            SL_DATAFORMAT_PCM,
            2,
//            SL_SAMPLINGRATE_44_1,
            (SLuint32) getCurrentSampleRateForOpensles(sample_rate),
            SL_PCMSAMPLEFORMAT_FIXED_16,
            SL_PCMSAMPLEFORMAT_FIXED_16,
            SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT,
            SL_BYTEORDER_LITTLEENDIAN
    };


//    const SLInterfaceID ids[1] = {SL_IID_BUFFERQUEUE};
//    const SLboolean req[1] = {SL_BOOLEAN_TRUE};
//    使用控制音量需要修改为
    const SLInterfaceID ids[4] = {SL_IID_BUFFERQUEUE, SL_IID_VOLUME, SL_IID_MUTESOLO,
                                  SL_IID_PLAYBACKRATE};
    const SLboolean req[4] = {SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE,
                              SL_BOOLEAN_TRUE};

//    const SLInterfaceID ids[3] = {SL_IID_BUFFERQUEUE, SL_IID_EFFECTSEND, SL_IID_VOLUME};
//    const SLboolean req[3] = {SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE};

    SLDataSource slDataSource = {&android_queue, &pcm};
    (*engineEngine)->CreateAudioPlayer(engineEngine, &pcmPlayerObj, &slDataSource, &slDataSink,
                                       4,
                                       ids, req);


    (*pcmPlayerObj)->Realize(pcmPlayerObj, SL_BOOLEAN_FALSE);
    (*pcmPlayerObj)->GetInterface(pcmPlayerObj, SL_IID_PLAY, &pcmPlayer);

    //获取音量控制
    (*pcmPlayerObj)->GetInterface(pcmPlayerObj, SL_IID_VOLUME, &pcmVolumePlay);
    //获取声道接口
    (*pcmPlayerObj)->GetInterface(pcmPlayerObj, SL_IID_MUTESOLO, &pcmMuteSoloPlay);

    //-------------------------------------------
    //设置缓冲
    (*pcmPlayerObj)->GetInterface(pcmPlayerObj, SL_IID_BUFFERQUEUE, &pcmBufferQueue);

    //如果有需求可以，在这是默认音量
    setVolume(defaultVolume);

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

void WlAudio::pause() {
    if (pcmPlayer == NULL)
        return;
    (*pcmPlayer)->SetPlayState(pcmPlayer, SL_PLAYSTATE_PAUSED);

}

void WlAudio::resume() {
    if (pcmPlayer == NULL)
        return;
    (*pcmPlayer)->SetPlayState(pcmPlayer, SL_PLAYSTATE_PLAYING);
}

void WlAudio::stop() {
    if (pcmPlayer == NULL)
        return;
    (*pcmPlayer)->SetPlayState(pcmPlayer, SL_PLAYSTATE_STOPPED);

}

void WlAudio::release() {
    //停止音频
    stop();
    if(queue!=NULL){
        queue->notifyQueue();
    }
//    pthread_join(pcmCallbackThread,NULL);
    pthread_join(playThread,NULL);
    //释放队列
    if (queue != NULL) {
        delete (queue);
        queue = NULL;
    }
    //释放博放器
    if (pcmPlayerObj != NULL) {
        (*pcmPlayerObj)->Destroy(pcmPlayerObj);
        pcmPlayerObj = NULL;

        pcmPlayer = NULL;
        pcmVolumePlay = NULL;
        pcmBufferQueue = NULL;

        pcmMuteSoloPlay = NULL;
    }

    if (outPutMixObject != NULL) {
        (*outPutMixObject)->Destroy(outPutMixObject);
        outPutMixObject = NULL;
        outEnvironmentalReverbItf = NULL;
    }

    if (engineObject != NULL) {
        (*engineObject)->Destroy(engineObject);
        engineObject = NULL;
        engineEngine = NULL;
    }
    //释放buffer
    if (buffer != NULL) {
        free(buffer);
        buffer = NULL;
    }

    if (out_buffer != NULL) {
        out_buffer = NULL;
    }

    if (soundTouch != NULL) {
        delete (soundTouch);
        soundTouch = NULL;
    }

    if (sampleBuffer != NULL) {
        free(sampleBuffer);
        sampleBuffer = NULL;
    }

    //释放解码器上下文
    if (avCodecContext != NULL) {
        pthread_mutex_lock(&codecMutex);
        avcodec_close(avCodecContext);
        avcodec_free_context(&avCodecContext);
        avCodecContext = NULL;
        pthread_mutex_unlock(&codecMutex);
    }

    //释放创建的对象
    if (playState != NULL) {
        playState = NULL;
    }

    if (callJava != NULL) {
        callJava = NULL;
    }

    if (bufferQueue != NULL) {
        bufferQueue->noticeThread();
        pthread_join(pcmCallbackThread, NULL);
        bufferQueue->release();
        delete (bufferQueue);
        bufferQueue = NULL;
    }
}

WlAudio::~WlAudio() {
    pthread_mutex_destroy(&codecMutex);
}

void WlAudio::setVolume(int percent) {
    defaultVolume = percent;
    if (pcmVolumePlay != NULL) {
//        (*pcmVolumePlay)->SetVolumeLevel(pcmVolumePlay, (100 - percent) * -50);
        if (percent > 30) {
            (*pcmVolumePlay)->SetVolumeLevel(pcmVolumePlay, (100 - percent) * -20);
        } else if (percent > 25) {
            (*pcmVolumePlay)->SetVolumeLevel(pcmVolumePlay, (100 - percent) * -22);
        } else if (percent > 20) {
            (*pcmVolumePlay)->SetVolumeLevel(pcmVolumePlay, (100 - percent) * -25);
        } else if (percent > 15) {
            (*pcmVolumePlay)->SetVolumeLevel(pcmVolumePlay, (100 - percent) * -28);
        } else if (percent > 10) {
            (*pcmVolumePlay)->SetVolumeLevel(pcmVolumePlay, (100 - percent) * -30);
        } else if (percent > 5) {
            (*pcmVolumePlay)->SetVolumeLevel(pcmVolumePlay, (100 - percent) * -34);
        } else if (percent > 3) {
            (*pcmVolumePlay)->SetVolumeLevel(pcmVolumePlay, (100 - percent) * -37);
        } else if (percent > 0) {
            (*pcmVolumePlay)->SetVolumeLevel(pcmVolumePlay, (100 - percent) * -40);
        } else {
            (*pcmVolumePlay)->SetVolumeLevel(pcmVolumePlay, (100 - percent) * -100);
        }
    }

}

void WlAudio::setMute(int type) {
    if (pcmMuteSoloPlay == NULL) return;
    if (type == 0) { //左声道
        (*pcmMuteSoloPlay)->SetChannelMute(pcmMuteSoloPlay, 1, false);
        (*pcmMuteSoloPlay)->SetChannelMute(pcmMuteSoloPlay, 0, true);
    } else if (type == 1) {//右声道
        (*pcmMuteSoloPlay)->SetChannelMute(pcmMuteSoloPlay, 1, true);
        (*pcmMuteSoloPlay)->SetChannelMute(pcmMuteSoloPlay, 0, false);
    } else if (type == 2) {//立体声
        (*pcmMuteSoloPlay)->SetChannelMute(pcmMuteSoloPlay, 1, false);
        (*pcmMuteSoloPlay)->SetChannelMute(pcmMuteSoloPlay, 0, false);
    }

}

void WlAudio::setPitch(double newPitch) {
    if (soundTouch != NULL) {
        soundTouch->setPitch(newPitch);
    }

}

void WlAudio::setTempo(double newTempo) {
    if (soundTouch != NULL) {
        soundTouch->setTempo(newTempo);
    }
}

int WlAudio::getPCMDB(char *pcmdata, size_t pcmsize) {
    int db = 0;
    short int prevalue = 0;
    double sum = 0;
    for (int i = 0; i < pcmsize; i += 2) {

        memcpy(&prevalue, pcmdata + i, 2);
        sum += abs(prevalue);
    }
    sum = sum / (data_size / 2);

    if (sum > 0) {
        db = 20.0 * log10(sum);
    }

    return db;
}
