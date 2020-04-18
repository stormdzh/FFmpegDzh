//
// Created by tal on 2020-04-18.
//


#include "WlFFmpeg.h"

WlFFmpeg::WlFFmpeg(WlCallJava *callJava, const char *url) {
    this->callJava = callJava;
    this->url = url;
}

void *decodeFFmpeg(void *data) {


    WlFFmpeg *wlFFmpeg = (WlFFmpeg *) data;
    wlFFmpeg->decodeFFmpegThread();
    pthread_exit(&wlFFmpeg->decodeThread);
}

void WlFFmpeg::prepare() {

    pthread_create(&decodeThread, NULL, decodeFFmpeg, this);
}

void WlFFmpeg::decodeFFmpegThread() {

    av_register_all();
    avformat_network_init();
    this->pFormatCtx = avformat_alloc_context();
    //打开输入流文件
    int code_open_input = avformat_open_input(&pFormatCtx, url, NULL, NULL);
    if (code_open_input != 0) {
        char *string = av_err2str(code_open_input);
        LOGE("avformat_open_input 失败:%s", string);
        return;
    }

    int code_find_stream_info = avformat_find_stream_info(pFormatCtx, NULL);
    if (code_find_stream_info != 0) {
        LOGE("code_find_stream_info 失败");
        return;
    } else {
        LOGE("code_find_stream_info 成功");
    }
    //循环找流信息
    for (int i = 0; i < pFormatCtx->nb_streams; i++) {
        AVCodecParameters *pParameters = pFormatCtx->streams[i]->codecpar;
        //当前流是音频
        if (pParameters->codec_type == AVMEDIA_TYPE_AUDIO) {
            if (audio == NULL) {
                audio = new WlAudio();
                audio->streamIndex = i;
                audio->codecpar = pParameters;
            }
        }

    }
    //获取解码器
    AVCodec *pCodec = avcodec_find_decoder(audio->codecpar->codec_id);
    if (pCodec == NULL) {
        LOGE("avcodec_find_decoder 失败");
        return;
    } else {
        LOGE("avcodec_find_decoder 成功");
    }

    audio->avCodecContext = avcodec_alloc_context3(pCodec);
    if (audio->avCodecContext == NULL) {
        LOGE("avcodec_alloc_context3 失败");
        return;
    } else {
        LOGE("avcodec_alloc_context3 成功");
    }

    int code_parameters_to_context = avcodec_parameters_to_context(audio->avCodecContext,
                                                                   audio->codecpar);

    if (code_parameters_to_context < 0) {
        LOGE("code_parameters_to_context 失败");
        return;
    } else {
        LOGE("code_parameters_to_context 成功");
    }


    int code_avcodec_open = avcodec_open2(audio->avCodecContext, pCodec, 0);
    if (code_avcodec_open < 0) {
        LOGE("code_avcodec_open 失败");
        return;
    } else {
        LOGE("code_avcodec_open 成功");
    }
    LOGE("音频准备完毕");
    callJava->onCallPrepare(CHILD_THREAD);

}

void WlFFmpeg::start() {
    if (audio == NULL) {
        return;
    }

    int count = 0;
    while (1) {
        AVPacket *pPacket = av_packet_alloc();
        if (av_read_frame(pFormatCtx, pPacket) == 0) {

            count++;
            if (pPacket->stream_index == audio->streamIndex) {
                LOGE("解码帧：%d", count);

            } else {
                av_packet_free(&pPacket);
                av_free(pPacket);
            }
        } else {
            av_packet_free(&pPacket);
            av_free(pPacket);
            break;
        }

    }

}
