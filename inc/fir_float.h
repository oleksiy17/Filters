#ifndef __FIR_FLOAT_H_
#define __FIR_FLOAT_H_

#include "ring_buffer.h"
#include "math.h"

#define M2_PI 6.283185307179
#define TAP_64 65

static float h[TAP_64];

void FIR_coeff_calc(float* h, float sampRate, float cof1, float cof2);
void FIR_tration(float* h, float* buffer, CircularBuffer* circ_Config, FILE* ptrNewWavFile, FILE* ptrWavFile, int sampleNum);

#endif // !__FIR_FLOAT_H_



