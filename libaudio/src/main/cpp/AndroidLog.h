//
// Created by tal on 2020-04-15.
//

#ifndef FFMPEGDZH_ANDROIDLOG_H
#define FFMPEGDZH_ANDROIDLOG_H

#endif //FFMPEGDZH_ANDROIDLOG_H

#define LOG_DEBUG true

#define TAG "MFFMPEG"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, TAG, __VA_ARGS__)
