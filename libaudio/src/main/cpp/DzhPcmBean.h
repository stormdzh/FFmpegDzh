//
// Created by dzh on 2020-04-20.
// 音频数据实体类
//

#ifndef FFMPEGDZH_DZHPCMBEAN_H
#define FFMPEGDZH_DZHPCMBEAN_H

#include "SoundTouch.h"

using namespace soundtouch;

class DzhPcmBean {
public:
    char *buffer;
    int buffsize;
public:
    DzhPcmBean(SAMPLETYPE *buffer, int size);

    ~DzhPcmBean();
};

#endif //FFMPEGDZH_DZHPCMBEAN_H
