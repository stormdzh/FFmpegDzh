//
// Created by tal on 2020-04-18.
//

#include "TestSLES.h"

SLAndroidSimpleBufferQueueItf pcmBufferQueue;

FILE *pcmFile;
void *buffer;
uint8_t *out_buffer;

int getPcmDada(void **pcm) {
    int size = 0;
    while (!feof(pcmFile)) {
        size = fread(out_buffer, 1, 44100 * 2 * 2, pcmFile);
        if (out_buffer == NULL) {
            LOGE("读完了");
            break;
        } else {
            LOGE("读取中。。。");
        }
        *pcm = out_buffer;
        break;
    }
    return size;
}

void pcmBufferCallBack(SLAndroidSimpleBufferQueueItf bf, void *context) {
    int size = getPcmDada(&buffer);
    if (buffer != NULL) {
        (*pcmBufferQueue)->Enqueue(pcmBufferQueue, buffer, size);
    }
}


TestSLES::TestSLES(const char *url) {

    pcmFile = fopen(url, "r");
    if (pcmFile == NULL) {
        return;
    }

    out_buffer = (uint8_t *) malloc(44100 * 2 * 2);

    //-------------------------------------------
    SLObjectItf engineObject = NULL;
    SLEngineItf engineEngine = NULL;

    slCreateEngine(&engineObject, 0, 0, 0, 0, 0);
    (*engineObject)->Realize(engineObject, SL_BOOLEAN_FALSE);
    (*engineObject)->GetInterface(engineObject, SL_IID_ENGINE, &engineEngine);


    //-------------------------------------------
    SLObjectItf outPutMixObject=NULL;
    SLEnvironmentalReverbItf outEnvironmentalReverbItf = NULL;

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
    SLObjectItf pcmPlayerObj = NULL;
    SLPlayItf pcmPlayer=NULL;
    SLDataLocator_OutputMix outPutMix = {SL_DATALOCATOR_OUTPUTMIX, outPutMixObject};
    SLDataSink slDataSink = {&outPutMix, NULL};

    SLDataLocator_AndroidBufferQueue android_queue = {SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE, 2};
    SLDataFormat_PCM pcm = {
            SL_DATAFORMAT_PCM,
            2,
            SL_SAMPLINGRATE_44_1,
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
//    SLAndroidSimpleBufferQueueItf pcmBufferQueue;
    (*pcmPlayerObj)->GetInterface(pcmPlayerObj, SL_IID_BUFFERQUEUE, &pcmBufferQueue);
    (*pcmBufferQueue)->RegisterCallback(pcmBufferQueue, pcmBufferCallBack, NULL);
    (*pcmPlayer)->SetPlayState(pcmPlayer, SL_PLAYSTATE_PLAYING);

    pcmBufferCallBack(pcmBufferQueue, NULL);

}
