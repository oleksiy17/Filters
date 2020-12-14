#ifndef __CROSSOVER_DATATYPES_H_
#define __CROSSOVER_DATATYPES_H_

#include "fixedpoint.h"

#define M_PI_cr        3.1415926535f
#define Q_cr           0.70710678118f

typedef struct {
    my_sint32 L;
    my_sint32 R;
}crosStereo;

typedef struct {
    my_float cutoff_freq;
    my_float samplerate;
}crossover_params;

typedef struct {
    my_sint32 k0;
    my_sint32 k1;
    my_sint32 k2;
}crossover_coeffs;

typedef struct {
    crosStereo x0_ord1;
    crosStereo y0_ord1;
    crosStereo z_x1_ord1;
    crosStereo z_y1_ord1;
    
    crosStereo x0_ord2;
    crosStereo y0_ord2;
    crosStereo z_x1_ord2;
    crosStereo z_x2_ord2;
    crosStereo z_y1_ord2;
    crosStereo z_y2_ord2;
}crossover_states;



#endif // !__CROSSOVER_DATATYPES_H_
