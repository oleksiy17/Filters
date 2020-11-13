#ifndef __CROSSOVER_DATATYPES_H_
#define __CROSSOVER_DATATYPES_H_

#include "fixedpoint.h"

#define M_PI        3.1415926535f
#define Q           0.70710678118f
#define BAND_NUM    4u

typedef enum {
    CUTOFF_0 = 0,
    CUTOFF_1 = 1,
    CUTOFF_2 = 2,
    SAMPLERATE = 3

}crossover_params_enum;


typedef struct {
    my_float L;
    my_float R;
}tStereo;

typedef struct {
    tStereo LP;
    tStereo HP;
}band_split;

typedef struct {
    tStereo band_1;
    tStereo band_2;
    tStereo band_3;
    tStereo band_4;
};

/*
*   PARAMETERS
*/
typedef struct {
    float cutoff_freq[BAND_NUM - 1];
    float samplerate;
}crossover_params;


/*
*   COEFFICIENTS
*/
typedef struct {
    float k1_1st;

    float k1_2nd;
    float k2_2nd;
}band_coeffs;

typedef struct {
    band_coeffs coeff_band[BAND_NUM - 1];
}crossover_coeffs;


/*
*   STATES
*/

//states for one apf uint contains apf of 1-st and 2-nd order


typedef struct {


}crossover_states; // size = 18*3 + 2*6 = 54 + 12 = 66 floats



#endif // !__CROSSOVER_DATATYPES_H_
