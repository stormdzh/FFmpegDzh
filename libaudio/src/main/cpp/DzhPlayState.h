//
// Created by dzh on 2020-04-18.
//

#ifndef FFMPEGDZH_DZHPLAYSTATE_H
#define FFMPEGDZH_DZHPLAYSTATE_H

class DzhPlayState{
public:
    bool exit;
    bool load = true;
    bool isSeeking= false; //正在seek，停止解码
    //暂停
    bool pause=false;
public:
    DzhPlayState();
    ~DzhPlayState();
};

#endif //FFMPEGDZH_DZHPLAYSTATE_H