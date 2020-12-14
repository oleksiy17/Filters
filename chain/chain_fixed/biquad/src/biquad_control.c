#include <stdint.h>
#include <stddef.h>
#include "math.h"

#include "biquad_control.h"

/*******************************************************************************
 * Provides with the required data sizes for parameters and coefficients.
 *   It is caller responsibility to allocate enough memory (bytes) for them.
 * 
 * @param[out] params_bytes   required data size for storing parameters
 * @param[out] coeffs_bytes   required data size for storing coefficients
 * 
 * @return 0 if success, non-zero error code otherwise
 ******************************************************************************/
int32_t biquad_control_get_sizes(
    size_t*     params_bytes,
    size_t*     coeffs_bytes)
{
    *params_bytes = sizeof(biquad_params);
    *coeffs_bytes = sizeof(biquad_coeffs);
    return 0;
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
int32_t biquad_control_initialize(
    void*       params,
    void*       coeffs,
    uint32_t    sample_rate)
{
    biquad_params *init_params;
    biquad_coeffs *init_coeffs;
    size_t i;

    init_params = (biquad_params*)params;
    init_coeffs = (biquad_coeffs*)coeffs;
    init_params->sample_rate = sample_rate;
    
    init_params->freq = 0;
    init_params->gain = 0;
    init_params->Q_bq = 0;
    init_params->type = 5;
    init_params->bypass = 0;

    init_coeffs->b0 = 0;
    init_coeffs->b1 = 0;
    init_coeffs->b2 = 0;

    init_coeffs->a0 = 0;
    init_coeffs->a1 = 0;
    init_coeffs->a2 = 0;

    return 0;
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
int32_t biquad_set_parameter(
    void*       params,
    int32_t     id,
    float       value)
{
    biquad_params *set_params = (biquad_params*)params;

    set_params->freq = 1000;
    set_params->gain = -6;
    set_params->Q_bq = 6;
    set_params->type = 2;
    set_params->sample_rate = 48000;

    return 0;
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
int32_t biquad_update_coeffs(
    void const* params,
    void*       coeffs)
{
    double  A,  sn, cs, alpha, beta, omega;
    double  b0, b1, b2, a0,    a1,   a2;

    biquad_params *update_params;
    biquad_coeffs *update_coeffs;

    update_params = (biquad_params*)params;
    update_coeffs = (biquad_coeffs*)coeffs;


    A        = pow(10, update_params->gain / 40);
    omega    = 2 * M_PI_bq * update_params->freq / update_params->sample_rate;
    sn       = sin(omega);
    cs       = cos(omega);
    alpha    = sn / (2 * update_params->Q_bq);
    beta     = sqrt(A + A);

    switch ((my_sint32)update_params->type)
    {
        case LOWPASS:
        {
            b0 = (1.0 - cs) / 2.0;
            b1 = 1.0 - cs;
            b2 = (1.0 - cs) / 2.0;
            a0 = 1.0 + alpha;
            a1 = -2.0 * cs;
            a2 = 1.0 - alpha;
            break;
        }
        
        case HIGHPASS:
        {
            b0 = (1 + cs) / 2.0;
            b1 = -(1 + cs);
            b2 = (1 + cs) / 2.0;
            a0 = 1 + alpha;
            a1 = -2 * cs;
            a2 = 1 - alpha;
            break;
        }
        
        case PEAK:
        {
            b0 = 1 + (alpha * A);
            b1 = -2 * cs;
            b2 = 1 - (alpha * A);
            a0 = 1 + (alpha / A);
            a1 = -2 * cs;
            a2 = 1 - (alpha / A);
            break;
        }
        
        case LOWSHELL:
        {
            b0 = A * ((A + 1) - (A - 1) * cs + beta * sn);
            b1 = A * ((A - 1) - (A + 1) * cs) * 2;
            b2 = A * ((A + 1) - (A - 1) * cs - beta * sn);
            a0 = (A + 1) + (A - 1) * cs + beta * sn;
            a1 = ((A - 1) + (A + 1) * cs) * (-2);
            a2 = (A + 1) + (A - 1) * cs - beta * sn;
            break;
        }
        
        case HIGHSHELL:
        {
            b0 = A * ((A + 1) + (A - 1) * cs + beta * sn);
            b1 = A * ((A - 1) + (A + 1) * cs) * (-2);
            b2 = A * ((A + 1) + (A - 1) * cs - beta * sn);
            a0 = (A + 1) - (A - 1) * cs + beta * sn;
            a1 = ((A - 1) - (A + 1) * cs) * 2;
            a2 = (A + 1) - (A - 1) * cs - beta * sn;
            break;
        }
        
        case DISABLE:
        {
            b0 = 0;
            b1 = 0;
            b2 = 0;
            a0 = 0;
            a1 = 0;
            a2 = 0;
            break;
        }
       
    }

    b0 /= a0;
    b1 /= a0;
    b2 /= a0;
    a1 /= a0;
    a2 /= a0;
    
    update_coeffs->a0 = double_To_Fixed((a0 / 8), Q31);
    update_coeffs->a1 = double_To_Fixed((a1 / 8), Q31);
    update_coeffs->a2 = double_To_Fixed((a2 / 8), Q31);
    update_coeffs->b0 = double_To_Fixed((b0 / 8), Q31);
    update_coeffs->b1 = double_To_Fixed((b1 / 8), Q31);
    update_coeffs->b2 = double_To_Fixed((b2 / 8), Q31);

    return 0;
}
