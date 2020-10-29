#ifndef __EFFECT_CONTROL_H__
#define __EFFECT_CONTROL_H__

#include <stdint.h>
#include <stddef.h>
#include "math.h"
#include "fixedpoint.h"

# define M_e    ((float)2.71828182846)

typedef struct {
    float threshold;
    float ratio;
    float tauAttack;
    float tauRelease;
    float makeUpGain;
    float samplerate;
    /*float kneeWidth;*/
}comprssor_params;

typedef struct {
    float threshold;
    float ratio;

    float alphaAttack;
    float alphaRelease;

    float makeUpGain;
    float samplerate;
}compressor_coeffs;

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
    *params_bytes = sizeof(comprssor_params);
    *coeffs_bytes = sizeof(compressor_coeffs);
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
    comprssor_params* init_params = (comprssor_params*)params;
    compressor_coeffs* init_coeffs = (compressor_coeffs*)coeffs;

    init_params->threshold = 0.0;
    init_params->ratio = 0.0;
    init_params->tauAttack = 0.0;
    init_params->tauRelease = 0.0;
    init_params->makeUpGain = 0.0;
    init_params->samplerate = 0.0;

    init_coeffs->threshold = 0.0;
    init_coeffs->ratio = 0.0;
    init_coeffs->alphaAttack = 0.0;
    init_coeffs->alphaRelease = 0.0;
    init_coeffs->makeUpGain = 0.0;
    init_coeffs->samplerate = 0.0;
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
    comprssor_params* set_params = (comprssor_params*)params;

    switch (id)
    {
        case 0:
        {
            set_params->threshold = value;
            break;
        }

        case 1:
        {
            set_params->ratio = value;
            break;
        }

        case 2:
        {
            set_params->tauAttack = value;
            break;
        }

        case 3:
        {
            set_params->tauRelease = value;
            break;
        }

        case 4:
        {
            set_params->makeUpGain = value;
            break;
        }
        case 5:
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
    comprssor_params* update_params = (comprssor_params*)params;
    compressor_coeffs* update_coeffs = (compressor_coeffs*)coeffs;

    update_coeffs->threshold = update_params->threshold;
    update_coeffs->ratio = update_params->ratio;

    update_coeffs->alphaAttack = powf(M_e, ((-2.2) / (0.001*update_params->tauAttack*update_params->samplerate)));
    update_coeffs->alphaRelease = powf(M_e, ((-2.2) / (0.001*update_params->tauRelease*update_params->samplerate)));

    /*update_coeffs->alphaAttack = 1 - powf(M_e, ((-2.2) / (0.001*update_params->tauAttack*update_params->samplerate)));
    update_coeffs->alphaRelease = 1 - powf(M_e, ((-2.2) / (0.001*update_params->tauRelease*update_params->samplerate)));*/

    update_coeffs->makeUpGain = update_params->makeUpGain;
    update_coeffs->samplerate = update_params->samplerate;
}


#endif