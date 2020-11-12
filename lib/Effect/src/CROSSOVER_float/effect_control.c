#ifndef __EFFECT_CONTROL_H__
#define __EFFECT_CONTROL_H__

#include <stdint.h>
#include <stddef.h>
#include "math.h"

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
    float cutoff_freq[BAND_NUM - 1];
    float samplerate;
}crossover_params;

typedef struct {
    float k1_1st;

    float k1_2nd;
    float k2_2nd;
}coeffs_apf;

typedef struct {
    coeffs_apf coeff[BAND_NUM - 1];
}crossover_coeffs;

/*******************************************************************************
 * Provides with the required data sizes for parameters and coefficients.
 *   It is caller responsibility to allocate enough memory (bytes) for them.
 * 
 * @param[out] params_bytes   required data size for storing parameters
 * @param[out] coeffs_bytes   required data size for storing coefficients
 * 
 * @return 0 if success, non-zero error code otherwise
 ******************************************************************************/
int32_t effect_control_get_sizes(
    size_t*     params_bytes,
    size_t*     coeffs_bytes)
{
    *params_bytes = sizeof(crossover_params);
    *coeffs_bytes = sizeof(crossover_coeffs);
}


/*******************************************************************************
 * Initializes params, coeffs and states to default values for the requested SR.
 * 
 * @param[in]  params       pointer to the pre-allocated params
 * @param[in]  coeffs       pointer to the pre-allocated coeffs
 * @param[in]  sample_rate  sampling rate
 * 
 * @return 0 if effect is initialized, non-zero error code otherwise
 ******************************************************************************/
int32_t effect_control_initialize(
    void*       params,
    void*       coeffs,
    uint32_t    sample_rate)
{
    crossover_params* init_params;
    crossover_coeffs* init_coeffs;
    uint32_t i;

    init_params = (crossover_params*)params;
    init_coeffs = (crossover_coeffs*)coeffs;

    init_params->cutoff_freq[0] = 0.0;
    init_params->cutoff_freq[1] = 0.0;
    init_params->cutoff_freq[2] = 0.0;
    init_params->samplerate = sample_rate;

    for (i = 0; i < (BAND_NUM - 1); i++)
    {
        init_coeffs->coeff[i].k1_1st = 0.0;
        init_coeffs->coeff[i].k1_2nd = 0.0;
        init_coeffs->coeff[i].k2_2nd = 0.0;
    }
    
}


/*******************************************************************************
 * Set single parameter and calculate corresponding coefficients.
 * 
 * @param[in] params    initialized params
 * @param[in] id        parameter ID
 * @param[in] value     parameter value
 * 
 * @return 0 if success, non-zero error code otherwise
 ******************************************************************************/
int32_t effect_set_parameter(
    void*       params,
    int32_t     id,
    float       value)
{
    crossover_params* set_params;

    set_params = (crossover_params*)params;

    switch (id)
    {
        case CUTOFF_0:
        {
            set_params->cutoff_freq[CUTOFF_0] = value;
            break;
        }

        case CUTOFF_1:
        {
            set_params->cutoff_freq[CUTOFF_1] = value;
            break;
        }

        case CUTOFF_2:
        {
            set_params->cutoff_freq[CUTOFF_2] = value;
            break;
        }

        case SAMPLERATE:
        {
            set_params->samplerate = value;
            break;
        }

        default:
            break;
    }
}


/*******************************************************************************
 * Calculate coefficients, corresponding to the parameters.
 * 
 * @param[in] params    initialized params
 * @param[in] id        parameter ID
 * @param[in] value     parameter value
 * 
 * @return 0 if success, non-zero error code otherwise
 ******************************************************************************/
int32_t effect_update_coeffs(
    void const* params,
    void*       coeffs)
{
    crossover_params* init_params;
    crossover_coeffs* init_coeffs;
    uint32_t i;

    float fb;
    float var1;
    float var2;

    init_params = (crossover_params*)params;
    init_coeffs = (crossover_coeffs*)coeffs;

    
    for (i = 0; i < (BAND_NUM - 1); i++)
    {
        fb = init_params->cutoff_freq[i] / (float)Q;
        var1 = (tanf(M_PI * (fb / init_params->samplerate)) - 1.0) / (tanf(M_PI * (fb / init_params->samplerate)) + 1.0);
        var2 = -cosf(2.0 * M_PI * init_params->cutoff_freq[i] / init_params->samplerate);

        init_coeffs->coeff[i].k1_1st = (tanf(M_PI * (init_params->cutoff_freq[i] / init_params->samplerate)) - 1.0) / (tanf(M_PI * (init_params->cutoff_freq[i] / init_params->samplerate)) + 1.0);
        init_coeffs->coeff[i].k1_2nd = var2 * (1.0 - var1);
        init_coeffs->coeff[i].k2_2nd = -var1;
    }

}


#endif
