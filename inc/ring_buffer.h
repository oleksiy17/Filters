#ifndef __RING_BUFFER_H_
#define __RING_BUFFER_H_

#include "stdlib.h"
#include "stdio.h"
#include "stdint.h"

#include "fixedpoint.h"
typedef struct {
    void* current;
    void* begin;
    void* end;
}CircularBuffer;

/*typedef struct {
    float* current;
    float* begin;
    float* end;
}CircularBuffer;*/

void Init_CircBuff(float* buf, FILE* readFile, CircularBuffer* circ_Config, size_t* numRead);
void Update_CircBuff(float* buf, FILE* readFile, CircularBuffer* circ_Config);
void SetCurrent_CircBuff(CircularBuffer* circ_Config);
void Convolution_CircBuff(CircularBuffer* circ_Config);

void Init_CircBuff_fxd(my_sint32* buf, FILE* readFile, CircularBuffer* circ_Config, size_t* numRead);
void Update_CircBuff_fxd(my_sint32* buf, FILE* readFile, CircularBuffer* circ_Config);
void SetCurrent_CircBuff_fxd(CircularBuffer* circ_Config);
void Convolution_CircBuff_fxd(CircularBuffer* circ_Config);

#endif // !__RING_BUFFER_H_
