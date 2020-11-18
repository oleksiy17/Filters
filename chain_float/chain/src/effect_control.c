#include <stdint.h>
#include <stddef.h>

#include "effect_control.h"




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
    *params_bytes = sizeof(chain_params);
    *coeffs_bytes = sizeof(chain_coeffs);
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
    chain_params* init_params = (chain_params*)params;
    chain_coeffs* init_coeffs = (chain_coeffs*)coeffs;

    compressor_4ch_control_initialize((comprssor_4ch_params*)&init_params->compr_4ch_par, (compressor_4ch_coeffs*)&init_coeffs->compr_4ch_coef, sample_rate);
    crossover_control_initialize((crossover_params*)&init_params->cross_par, (crossover_coeffs*)&init_coeffs->cross_coef, sample_rate);
    equalizer_control_initialize((equalizer_params*)&init_params->equal_par, (equalizer_coeffs*)&init_coeffs->equal_coef, sample_rate);
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
    equalizer_set_parameter(params, id, value);
    compressor_4ch_set_parameter(params, id, value);
    crossover_set_parameter(params, id, value);
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
    chain_params* update_params = (chain_params*)params;
    chain_coeffs* update_coeffs = (chain_coeffs*)coeffs;

    compressor_4ch_update_coeffs((comprssor_4ch_params*)&update_params->compr_4ch_par, (compressor_4ch_coeffs*)&update_coeffs->compr_4ch_coef);
    crossover_update_coeffs((crossover_params*)&update_params->cross_par, (crossover_coeffs*)&update_coeffs->cross_coef);
    equalizer_update_coeffs((equalizer_params*)&update_params->equal_par, (equalizer_coeffs*)&update_coeffs->equal_coef);
}
