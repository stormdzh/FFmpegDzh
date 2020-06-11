//
// Created by dzh on 2020-04-18.
//

#ifndef FFMPEGDZH_DZHFFMPEG_H
#define FFMPEGDZH_DZHFFMPEG_H

#include "AndroidLog.h"
#include <android/log.h>
#include "pthread.h"
#include "DzhCallJava.h"
#include "DzhAudio.h"
#include "DzhVideo.h"

extern "C"
{
#include "libavformat/avformat.h"
#include <libavutil/time.h>
};

class DzhFFmpeg {
public:
    WlCallJava *callJava = NULL;
    const char *url = NULL;
    pthread_t decodeThread;
    AVFormatContext *pFormatCtx = NULL;
    DzhAudio *audio = NULL;
    DzhPlayState *playState = NULL;
    //初始化的锁
    pthread_mutex_t init_mutex;
    //seek的锁
    pthread_mutex_t seek_mutex;

    bool initexit = false;

    int duration = 0;

    //视频相关
    DzhVideo *video = NULL;

    //硬解码使用
    AVBitStreamFilter *bsfilter = NULL;

    bool isSupporMediaCodec=false;

public:

    DzhFFmpeg(DzhPlayState *playState, WlCallJava *callJava, const char *url);

    ~DzhFFmpeg();

    void prepare();

    void decodeFFmpegThread();

    void start();


    void pause();

    void resume();

    void release();

    //seek 单位是秒
    void seek(int64_t secds);

    void setVolume(int percent);

    void setMute(int type);


    //裁剪音频
    bool cutAudioPlay(int satrt_tiem, int end_time, bool show_pcm_data);

    //视频相关
    int getCodecContext(AVCodecParameters *codecpar, AVCodecContext **avCodecContext);
};

#endif //FFMPEGDZH_DZHFFMPEG_H
