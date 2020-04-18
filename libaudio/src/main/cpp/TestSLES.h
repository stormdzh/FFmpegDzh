//
// Created by tal on 2020-04-18.
//

#ifndef FFMPEGDZH_TESTSLES_H
#define FFMPEGDZH_TESTSLES_H


#include "string"
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>

#include "AndroidLog.h"
#include <android/log.h>

class TestSLES {

public:
    TestSLES(const char *url);

    ~TestSLES();
};

#endif //FFMPEGDZH_TESTSLES_H
