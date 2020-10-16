#include "fixedpoint.h"
#include "ring_buffer.h"

#define TAP_65 65

void FIR_tration_fxd(my_sint32* h, my_sint32* buffer, CircularBuffer* circ_Config, FILE* ptrNewWavFile, FILE* ptrWavFile, int sampleNum);