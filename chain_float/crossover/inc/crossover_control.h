#ifndef __CROSSOVER_CONTROL_H__
#define __CROSSOVER_CONTROL_H__

#include <stdint.h>
#include <stddef.h>

#include "fixedpoint.h"

#define M_PI        3.1415926535f
#define Q           0.70710678118f
#define BAND_NUM    4u

typedef enum {
    LOW_CUTOFF = 0,
    MID_CUTOFF = 1,
    HIG_CUTOFF = 2,
    SAMPLERATE = 3

}crossover_params_enum;

typedef enum {
    PAR_1 = 0,
    PAR_2 = 1,
    SERIAL = 2,
    NORM_LOW = 0,
    NORM_HIG = 1

}crossover_states_enum;


typedef struct {
    my_float L;
    my_float R;
}tStereo_cross;

typedef struct {
    tStereo_cross band_1;
    tStereo_cross band_2;
    tStereo_cross band_3;
    tStereo_cross band_4;
}split_band;

typedef struct {
    my_float cutoff_freq[BAND_NUM - 1];
    my_float samplerate;
}crossover_params;                          // parameters

typedef struct {
    my_float k0;
    my_float k1;
    my_float k2;

    my_float negk0;
    my_float negk1;
    my_float negk2;
}band_coeffs;

typedef struct {
    band_coeffs band[BAND_NUM - 1];
}crossover_coeffs;                          // coefficients

typedef struct {
    tStereo_cross in;

    tStereo_cross cd1_ord1[2];
    tStereo_cross cd1_delay_1[3];
    tStereo_cross cd1_delay_2[3];

    tStereo_cross norm_ord1[2];
    tStereo_cross norm_delay_1[2];
    tStereo_cross norm_delay_2[2];

    tStereo_cross cd2_ord1[2];
    tStereo_cross cd2_delay_1[3];
    tStereo_cross cd2_delay_2[3];

    tStereo_cross cd3_ord1[2];
    tStereo_cross cd3_delay_1[3];
    tStereo_cross cd3_delay_2[3];

}crossover_states;                          // states


/*******************************************************************************
 * Provides with the required data sizes for parameters and coefficients.
 *   It is caller responsibility to allocate enough memory (bytes) for them.
 * 
 * @param[out] params_bytes   required data size for storing parameters
 * @param[out] coeffs_bytes   required data size for storing coefficients
 * 
 * @return 0 if success, non-zero error code otherwise
 ******************************************************************************/
int32_t crossover_control_get_sizes(
    size_t*     params_bytes,
    size_t*     coeffs_bytes);


/*******************************************************************************
 * Initializes params, coeffs and states to default values for the requested SR.
 * 
 * @param[in]  params       pointer to the pre-allocated params
 * @param[in]  coeffs       pointer to the pre-allocated coeffs
 * @param[in]  sample_rate  sampling rate
 * 
 * @return 0 if effect is initialized, non-zero error code otherwise
 ******************************************************************************/
int32_t crossover_control_initialize(
    void*       params,
    void*       coeffs,
    uint32_t    sample_rate);


/*******************************************************************************
 * Set single parameter and calculate corresponding coefficients.
 * 
 * @param[in] params    initialized params
 * @param[in] id        parameter ID
 * @param[in] value     parameter value
 * 
 * @return 0 if success, non-zero error code otherwise
 ******************************************************************************/
int32_t crossover_set_parameter(
    void*       params,
    int32_t     id,
    float       value);


/*******************************************************************************
 * Calculate coefficients, corresponding to the parameters.
 * 
 * @param[in] params    initialized params
 * @param[in] id        parameter ID
 * @param[in] value     parameter value
 * 
 * @return 0 if success, non-zero error code otherwise
 ******************************************************************************/
int32_t crossover_update_coeffs(
    void const* params,
    void*       coeffs);


#endif