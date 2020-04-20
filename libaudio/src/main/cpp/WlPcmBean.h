//
// Created by tal on 2020-04-20.
//

#ifndef FFMPEGDZH_WLPCMBEAN_H
#define FFMPEGDZH_WLPCMBEAN_H

#include "SoundTouch.h"

using namespace soundtouch;

class WlPcmBean {
public:
    char *buffer;
    int buffsize;
public:
    WlPcmBean(SAMPLETYPE *buffer, int size);

    ~WlPcmBean();
};

#endif //FFMPEGDZH_WLPCMBEAN_H
