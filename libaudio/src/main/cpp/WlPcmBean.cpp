//
// Created by tal on 2020-04-20.
//

#include "WlPcmBean.h"

WlPcmBean::WlPcmBean(SAMPLETYPE *buffer, int size) {
    this->buffer = (char *) malloc(size);
    this->buffsize = size;
    memcpy(this->buffer, buffer, buffsize);

}

WlPcmBean::~WlPcmBean() {

    free(this->buffer);
    buffer == NULL;
}
