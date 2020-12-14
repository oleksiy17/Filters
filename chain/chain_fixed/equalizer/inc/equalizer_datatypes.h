#ifndef __EQUALIZER_DATATYPES_H_
#define __EQUALIZER_DATATYPES_H_

#include "../../biquad/inc/biquad_datatypes.h"
#define BANDS   10
#define M_PI_eq  3.14159265358979323846

typedef enum {
    FRIQUENCY = 0,
    GAIN = 1,
    QF = 2,
    TYPE = 3
}eParam;

typedef struct {
    my_sint32 L;
    my_sint32 R;
}eqStereo;

typedef struct {
    my_float sample_rate;
    biquad_params bq[BANDS];
    my_sint32 bypass;

} equalizer_params;

typedef struct {
    biquad_coeffs bq[BANDS];
    my_sint32 bypass;

} equalizer_coeffs;

typedef struct {
    biquad_states bq[BANDS];

} equalizer_states;


#endif // !__EQUALIZER_DATATYPES_H_
