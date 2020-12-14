#include <stdint.h>
#include <stddef.h>
#include "math.h"

#include "equalizer_control.h"

/*******************************************************************************
 * Provides with the required data sizes for parameters and coefficients.
 *   It is caller responsibility to allocate enough memory (bytes) for them.
 * 
 * @param[out] params_bytes   required data size for storing parameters
 * @param[out] coeffs_bytes   required data size for storing coefficients
 * 
 * @return 0 if success, non-zero error code otherwise
 ******************************************************************************/
int32_t equalizer_control_get_sizes(
    size_t*     params_bytes,
    size_t*     coeffs_bytes)
{
    *params_bytes = sizeof(equalizer_params);
    *coeffs_bytes = sizeof(equalizer_coeffs);
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
int32_t equalizer_control_initialize(
    void*       params,
    void*       coeffs,
    uint32_t    sample_rate)
{
    equalizer_params *init_params;
    equalizer_coeffs *init_coeffs;
    size_t i;

    init_params = (equalizer_params*)params;
    init_coeffs = (equalizer_coeffs*)coeffs;

    init_params->sample_rate = sample_rate;
    init_coeffs->bypass = 0;

    for (i = 0; i < (size_t)BANDS; i++)
    {   
        biquad_control_initialize(&init_params->bq[i], &init_coeffs->bq[i], sample_rate);
    }
    
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
int32_t equalizer_set_parameter(
    void*       params,
    int32_t     id,
    float       value)
{
    equalizer_params *set_params;
    size_t band;
    size_t param_num;

    set_params = (equalizer_params*)params;

    band = id >> 2;
    param_num = id - (4 * band);

    if (id != 40)
    {
        switch (param_num)
        {
        case FRIQUENCY:
        {
            set_params->bq[band].freq = value;
            break;
        }

        case GAIN:
        {
            set_params->bq[band].gain = value;
            break;
        }

        case QF:
        {
            set_params->bq[band].Q_bq = value;
            break;
        }

        case TYPE:
        {
            set_params->bq[band].type = value;
            break;
        }

        case 99:
        {
            set_params->bypass = value;
        }

        default:
            break;
        }
    }
    else 
    {
        set_params->sample_rate = value;
    }


    
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
int32_t equalizer_update_coeffs(
    void const* params,
    void*       coeffs)
{
    
    equalizer_params *update_params;
    equalizer_coeffs *update_coeffs;
    size_t i;

    update_params = (equalizer_params*)params;
    update_coeffs = (equalizer_coeffs*)coeffs;

    update_coeffs->bypass = update_params->bypass;



    for (i = 0; i < BANDS; i++)
    {
        biquad_update_coeffs(&update_params->bq[i], &update_coeffs->bq[i]);
    }

    return 0;
}
