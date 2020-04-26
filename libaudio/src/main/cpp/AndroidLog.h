//
// Created by tal on 2020-04-15.
//

#include <android/log.h>

#ifndef FFMPEGDZH_ANDROIDLOG_H
#define FFMPEGDZH_ANDROIDLOG_H

#define LOG_DEBUG true

//打印普通log
#define TAG "MFFMPEG"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, TAG, __VA_ARGS__)


//打印视频log
#define TAGV "MFFMPEGVIDEO"
#define LOGIV(...) __android_log_print(ANDROID_LOG_INFO, TAGV, __VA_ARGS__)
#define LOGEV(...) __android_log_print(ANDROID_LOG_ERROR, TAGV, __VA_ARGS__)
#define LOGDV(...) __android_log_print(ANDROID_LOG_DEBUG, TAGV, __VA_ARGS__)

#endif //FFMPEGDZH_ANDROIDLOG_H
