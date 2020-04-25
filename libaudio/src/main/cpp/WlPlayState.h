//
// Created by tal on 2020-04-18.
//

#ifndef FFMPEGDZH_WLPLAYSTATE_H
#define FFMPEGDZH_WLPLAYSTATE_H

class WlPlayState{
public:
    bool exit;
    bool load = true;
    bool isSeeking= false; //正在seek，停止解码
    //暂停
    bool pause=false;
public:
    WlPlayState();
    ~WlPlayState();
};

#endif //FFMPEGDZH_WLPLAYSTATE_H