//
// Created by tal on 2020-04-18.
//

#include "WlFFmpeg.h"

WlFFmpeg::WlFFmpeg(WlPlayState *playState, WlCallJava *callJava, const char *url) {
    this->playState = playState;
    this->callJava = callJava;
    this->url = url;
    pthread_mutex_init(&init_mutex, NULL);
    pthread_mutex_init(&seek_mutex, NULL);
}

void *decodeFFmpeg(void *data) {


    WlFFmpeg *wlFFmpeg = (WlFFmpeg *) data;
    wlFFmpeg->decodeFFmpegThread();
    pthread_exit(&wlFFmpeg->decodeThread);
}

void WlFFmpeg::prepare() {

    pthread_create(&decodeThread, NULL, decodeFFmpeg, this);
}


//加载过程中，退出中断锁，可以正常退出
int avformat_callback(void *ctx) {

    WlFFmpeg *fFmpeg = (WlFFmpeg *) ctx;
    if (fFmpeg->playState->exit) {
        return AVERROR_EOF;
    }
    return 0;
}

void WlFFmpeg::decodeFFmpegThread() {

    pthread_mutex_lock(&init_mutex);
    //ffmpeg 4.0以上去掉了av_register_all，不需要调用
//    av_register_all();
    avformat_network_init();
    pFormatCtx = avformat_alloc_context();

    //异常处理
    pFormatCtx->interrupt_callback.callback = avformat_callback;
    pFormatCtx->interrupt_callback.opaque = this;


    //打开输入流文件
    int code_open_input = avformat_open_input(&pFormatCtx, url, NULL, NULL);
    if (code_open_input != 0) {
        char *string = av_err2str(code_open_input);
        LOGE("avformat_open_input 失败:%s", string);
        pthread_mutex_unlock(&init_mutex);
        initexit = true;
        callJava->onCallError(CHILD_THREAD, 10001, av_err2str(code_open_input));
        return;
    } else {
        LOGE("avformat_open_input 成功");
    }

    int code_find_stream_info = avformat_find_stream_info(pFormatCtx, NULL);
    if (code_find_stream_info != 0) {
        LOGE("code_find_stream_info 失败");
        initexit = true;
        pthread_mutex_unlock(&init_mutex);
        callJava->onCallError(CHILD_THREAD, 10002, av_err2str(code_find_stream_info));
        return;
    } else {
        LOGE("code_find_stream_info 成功");
    }
    //循环找流信息
    for (int i = 0; i < pFormatCtx->nb_streams; i++) {
        AVCodecParameters *pParameters = pFormatCtx->streams[i]->codecpar;
        //当前流是音频
        if (pParameters->codec_type == AVMEDIA_TYPE_AUDIO) { //处理音频流
            if (audio == NULL) {
                audio = new WlAudio(playState, pFormatCtx->streams[i]->codecpar->sample_rate,
                                    callJava);
                audio->streamIndex = i;
                audio->codecpar = pParameters;
                //设置总时长  pFormatCtx->duration的单位是微妙，转为秒
                audio->duration = pFormatCtx->duration / AV_TIME_BASE;
                //在当前流里面，没一帧的几分之几
                audio->time_base = pFormatCtx->streams[i]->time_base;
                //当前类中记录音频长度
                duration = audio->duration;
            }
        } else if (pParameters->codec_type == AVMEDIA_TYPE_VIDEO) {
            if (video == NULL) {
                video = new WlVideo(playState, callJava);
                video->streamIndex = i;
                video->codecpar = pParameters;
                video->time_base = pFormatCtx->streams[i]->time_base;

                //处理音视频同步
                int num = pFormatCtx->streams[i]->avg_frame_rate.num;
                int den = pFormatCtx->streams[i]->avg_frame_rate.den;
                if(num != 0 && den != 0){
                    int fps=num/den;
                    video->defualtDelayTime=1.0/fps;
                }
            }
        }

    }

    if (audio != NULL) { //打开音频解码器
        getCodecContext(audio->codecpar, &audio->avCodecContext);
    }

    if (video != NULL) { //打开视频解码器
        getCodecContext(video->codecpar, &video->avCodecContext);
    }


    LOGE("音频准备完毕");
    callJava->onCallPrepare(CHILD_THREAD);
    pthread_mutex_unlock(&init_mutex);

}

void WlFFmpeg::start() {
    if (audio == NULL) {
        callJava->onCallError(CHILD_THREAD, 10007, "audio is null");
        return;
    }

    if (video == NULL) {
        callJava->onCallError(CHILD_THREAD, 10008, "video is null");
    }

    video->audio = audio;

    //调用播放
    audio->play();
    video->play();

    int count = 0;
    while (playState != NULL && !playState->exit) {

        if (playState->isSeeking) {
            //休眠100毫秒
            av_usleep(1000 * 100);
        }
        if (playState != NULL && playState->isSeeking) {
            //正在seek的时候，走下次解码
            continue;
        }
        //控制缓存队列最多缓存40帧
        if (audio->queue->getQueueSize() > 100) {
            //休眠100毫秒
            av_usleep(1000 * 100);
            continue;
        }

        AVPacket *pPacket = av_packet_alloc();
        pthread_mutex_lock(&seek_mutex);
        int code_read_frame = av_read_frame(pFormatCtx, pPacket);
        pthread_mutex_unlock(&seek_mutex);
        if (code_read_frame == 0) {

            count++;
            if (pPacket->stream_index == audio->streamIndex) { //音频流
                LOGE("解码帧：%d", count);

                audio->queue->putAvPacket(pPacket);
            } else if (pPacket->stream_index == video->streamIndex) {//视频流
                video->queue->putAvPacket(pPacket);
                LOGE("获取到视频流");
            } else {
                av_packet_free(&pPacket);
                av_free(pPacket);
            }
        } else {
            av_packet_free(&pPacket);
            av_free(pPacket);

            if (playState != NULL && !playState->exit) {
                if (audio->queue->getQueueSize() > 0) {
                    //休眠100毫秒
                    av_usleep(1000 * 100);
                    continue;
                } else {
                    playState->exit = true;
                    break;
                }
            }
        }

    }
    //当上面的代码break，后测试从队列里面获取AVPacket
//    while (audio->queue->getQueueSize() > 0) {
//        AVPacket *pPacket = av_packet_alloc();
//        audio->queue->getAvPacket(pPacket);
//        av_packet_free(&pPacket);
//        av_free(pPacket);
//        pPacket = NULL;
//
//    }

    if (callJava != NULL) {
        callJava->onCallComplete(CHILD_THREAD);
    }
    initexit = true;

    LOGE("解码完成");

}

void WlFFmpeg::pause() {
    if(playState!=NULL){
        playState->pause=true;
    }
    if (audio != NULL) {
        audio->pause();
    }

}

void WlFFmpeg::resume() {

    if(playState!=NULL){
        playState->pause= false;
    }

    if (audio != NULL) {
        audio->resume();
    }
}

void WlFFmpeg::release() {
    if (playState != NULL && playState->exit) {
        return;
    }
    playState->exit = true;

    pthread_mutex_lock(&init_mutex);

    int sleepCount = 0;
    while (!initexit) {
        if (sleepCount > 1000) {
            initexit = true;
        }
        LOGE("release  sleepCount %d", sleepCount);
        sleepCount++;
        av_usleep(1000 * 10);
    }

    if (audio != NULL) {
        audio->release();
        delete (audio);
        audio = NULL;
    }

    if (video != NULL) {
        video->release();
        delete (video);
        video = NULL;
    }

    if (pFormatCtx != NULL) {
        avformat_close_input(&pFormatCtx);
        avformat_free_context(pFormatCtx);
        pFormatCtx = NULL;
    }

    if (playState != NULL) {
        playState = NULL;
    }

    if (callJava != NULL) {
        callJava = NULL;
    }

    pthread_mutex_unlock(&init_mutex);

}

WlFFmpeg::~WlFFmpeg() {
    pthread_mutex_destroy(&init_mutex);
    pthread_mutex_destroy(&seek_mutex);
}

void WlFFmpeg::seek(int64_t secds) {

    if (duration <= 0) {
        return;
    }
    if (secds >= 0 && secds <= duration) {

        if (audio != NULL) {
            playState->isSeeking = true;
            audio->queue->clearAvpacket();
            audio->clock = 0;
            audio->last_time = 0;

            pthread_mutex_lock(&seek_mutex);
            int64_t rel = secds * AV_TIME_BASE;
            //去掉解码器中缓存的文件
            avcodec_flush_buffers(audio->avCodecContext);
            avformat_seek_file(pFormatCtx, -1, INT64_MIN, rel, INT64_MAX, 0);
            pthread_mutex_unlock(&seek_mutex);
            playState->isSeeking = false;
        }
    }
}

void WlFFmpeg::setVolume(int percent) {

    if (audio != NULL) {
        audio->setVolume(percent);
    }
}

void WlFFmpeg::setMute(int type) {
    if (audio != NULL) {
        audio->setMute(type);
    }

}

bool WlFFmpeg::cutAudioPlay(int satrt_tiem, int end_time, bool show_pcm_data) {
    if (satrt_tiem >= 0 && end_time < duration && satrt_tiem < end_time) {

        audio->isCut = true;
        audio->endTime = end_time;
        audio->showPcm = show_pcm_data;
        seek(satrt_tiem);

        return true;
    }

}

int WlFFmpeg::getCodecContext(AVCodecParameters *codecpar, AVCodecContext **avCodecContext) {
    //获取解码器
    AVCodec *pCodec = avcodec_find_decoder(codecpar->codec_id);
    if (pCodec == NULL) {
        LOGE("avcodec_find_decoder 失败");
        initexit = true;
        pthread_mutex_unlock(&init_mutex);
        callJava->onCallError(CHILD_THREAD, 10003, "find_decoder error");
        return -1;
    } else {
        LOGE("avcodec_find_decoder 成功");
    }

    *avCodecContext = avcodec_alloc_context3(pCodec);
    if (*avCodecContext == NULL) {
        LOGE("avcodec_alloc_context3 失败");
        initexit = true;
        pthread_mutex_unlock(&init_mutex);
        callJava->onCallError(CHILD_THREAD, 10004, "alloc_context error");
        return -1;
    } else {
        LOGE("avcodec_alloc_context3 成功");
    }

    int code_parameters_to_context = avcodec_parameters_to_context(*avCodecContext,
                                                                   codecpar);

    if (code_parameters_to_context < 0) {
        LOGE("code_parameters_to_context 失败");
        initexit = true;
        pthread_mutex_unlock(&init_mutex);
        callJava->onCallError(CHILD_THREAD, 10005, av_err2str(code_parameters_to_context));
        return -1;
    } else {
        LOGE("code_parameters_to_context 成功");
    }


    int code_avcodec_open = avcodec_open2(*avCodecContext, pCodec, 0);
    if (code_avcodec_open < 0) {
        LOGE("code_avcodec_open 失败");
        initexit = true;
        pthread_mutex_unlock(&init_mutex);
        callJava->onCallError(CHILD_THREAD, 10006, av_err2str(code_avcodec_open));
        return -1;
    } else {
        LOGE("code_avcodec_open 成功");
    }
    return 0;
}
