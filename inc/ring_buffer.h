#ifndef __RING_BUFFER_H_
#define __RING_BUFFER_H_

#include "stdlib.h"
#include "stdio.h"
#include "stdint.h"

typedef struct {
    float* current;
    float* begin;
    float* end;
}CircularBuffer;

void Init_CircBuff(float* buf, FILE* readFile, CircularBuffer* circ_Config, size_t* numRead);
void Update_CircBuff(float* buf, FILE* readFile, CircularBuffer* circ_Config);
void SetCurrent_CircBuff(CircularBuffer* circ_Config);
void Convolution_CircBuff(CircularBuffer* circ_Config);

#endif // !__RING_BUFFER_H_
