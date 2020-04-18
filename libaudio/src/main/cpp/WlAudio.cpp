//
// Created by tal on 2020-04-18.
//

#include "WlAudio.h"

WlAudio::WlAudio(WlPlayState *playState) {

    this->playState=playState;
    queue=new WlQueue(playState);

}
