#ifndef __BIQUAD_DATATYPES_H_
#define __BIQUAD_DATATYPES_H_

#include "fixedpoint.h"

typedef enum {
    LOWPASS = 0,
    HIGHPASS = 1,
    PEAK = 2,
    LOWSHELL = 3,
    HIGHSHELL = 4,
    DISABLE = 5,

    TOTAL_REGIMES = 6
}BQType;

typedef struct {
    my_sint32 L;
    my_sint32 R;
}bqStereo;

typedef struct {
    my_float  Q_bq;
    my_float  freq;
    my_float  gain;
    my_float  type;
    my_sint32 bypass;
    my_float sample_rate;
} biquad_params;

typedef struct {
    my_sint32  b0;
    my_sint32  b1;
    my_sint32  b2;
    my_sint32  a0;
    my_sint32  a1;
    my_sint32  a2;
    my_sint32 bypass;
} biquad_coeffs;

typedef struct {
    bqStereo x0;
    bqStereo x1;
    bqStereo x2;
    bqStereo y0;
    bqStereo y1;
    bqStereo y2;
    bqStereo error;
} biquad_states;


#endif // !__BIQUAD_DATATYPES_H_
