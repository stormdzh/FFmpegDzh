//
// Created by dzh on 2020-04-20.
//

#include "DzhPcmBean.h"

DzhPcmBean::DzhPcmBean(SAMPLETYPE *buffer, int size) {
    this->buffer = (char *) malloc(size);
    this->buffsize = size;
    memcpy(this->buffer, buffer, buffsize);

}

DzhPcmBean::~DzhPcmBean() {

    free(this->buffer);
    buffer == NULL;
}
