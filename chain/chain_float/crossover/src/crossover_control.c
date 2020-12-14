#include <stdint.h>
#include <stddef.h>

#include "crossover_control.h"
#include "stdio.h"
#include "math.h"

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
int32_t crossover_control_initialize(
    void*       params,
    void*       coeffs,
    uint32_t    sample_rate)
{
    crossover_params* init_params;
    crossover_coeffs* init_coeffs;
    uint32_t i;

    init_params = (crossover_params*)params;
    init_coeffs = (crossover_coeffs*)coeffs;

    init_params->cutoff_freq[LOW_CUTOFF] = 0.0;
    init_params->cutoff_freq[MID_CUTOFF] = 0.0;
    init_params->cutoff_freq[HIG_CUTOFF] = 0.0;
    init_params->samplerate = sample_rate;

    for (i = 0; i < (BAND_NUM - 1); i++)
    {
        init_coeffs->band[i].ord_1.k0 = 0.0;

        init_coeffs->band[i].ord_2.a0 = 0.0;
        init_coeffs->band[i].ord_2.a1 = 0.0;
        init_coeffs->band[i].ord_2.a2 = 0.0;
        init_coeffs->band[i].ord_2.b0 = 0.0;
        init_coeffs->band[i].ord_2.b1 = 0.0;
        init_coeffs->band[i].ord_2.b2 = 0.0;
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
int32_t crossover_set_parameter(
    void*       params,
    int32_t     id,
    float       value)
{
    crossover_params* set_params;

    set_params = (crossover_params*)params;

    switch (id)
    {
        case 200:
        {
            set_params->cutoff_freq[LOW_CUTOFF] = value;
            break;
        }

        case 201:
        {
            set_params->cutoff_freq[MID_CUTOFF] = value;
            break;
        }

        case 202:
        {
            set_params->cutoff_freq[HIG_CUTOFF] = value;
            break;
        }

        case 210:
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
int32_t crossover_update_coeffs(
    void const* params,
    void*       coeffs)
{
    crossover_params* update_params;
    crossover_coeffs* update_coeffs;
    uint32_t i;

    update_params = (crossover_params*)params;
    update_coeffs = (crossover_coeffs*)coeffs;

    double  A[BAND_NUM - 1], sn[BAND_NUM - 1], cs[BAND_NUM - 1], alpha[BAND_NUM - 1], beta[BAND_NUM - 1], omega[BAND_NUM - 1];
    double  b0[BAND_NUM - 1], b1[BAND_NUM - 1], b2[BAND_NUM - 1], a0[BAND_NUM - 1], a1[BAND_NUM - 1], a2[BAND_NUM - 1];
    
    for (i = 0; i < (BAND_NUM - 1); i++)
    {
        update_coeffs->band[i].ord_1.k0 = (tanf(M_PI * (update_params->cutoff_freq[i] / update_params->samplerate)) - 1.0) / (tanf(M_PI * (update_params->cutoff_freq[i] / update_params->samplerate)) + 1.0);

        omega[i] = 2 * M_PI * update_params->cutoff_freq[i] / update_params->samplerate;
        sn[i] = sin(omega[i]);
        cs[i] = cos(omega[i]);
        alpha[i] = sn[i] / (2 * new_Q);
        beta[i] = sqrt(A[i] + A[i]);

        b0[i] = 1.0 - alpha[i];
        b1[i] = -2.0 * cs[i];
        b2[i] = 1.0 + alpha[i];
        a0[i] = 1.0 + alpha[i];
        a1[i] = -2.0 * cs[i];
        a2[i] = 1.0 - alpha[i];

        b0[i] /= a0[i];
        b1[i] /= a0[i];
        b2[i] /= a0[i];
        a1[i] /= a0[i];
        a2[i] /= a0[i];

        update_coeffs->band[i].ord_2.a0 = (float)(a0[i]); // /8
        update_coeffs->band[i].ord_2.a1 = -(float)(a1[i]);
        update_coeffs->band[i].ord_2.a2 = -(float)(a2[i]);
        update_coeffs->band[i].ord_2.b0 = (float)(b0[i]);
        update_coeffs->band[i].ord_2.b1 = (float)(b1[i]);
        update_coeffs->band[i].ord_2.b2 = (float)(b2[i]);
        
           
    }

    printf("\n");
    for (int r = 0; r < 3; r++)
    {
        printf("%34.100f, ", update_coeffs->band[r].ord_2.b0);
    }
    printf("*\n");

    for (int r = 0; r < 3; r++)
    {
        printf("%34.100f, ", update_coeffs->band[r].ord_2.b1);
    }
    printf("*\n");

    for (int r = 0; r < 3; r++)
    {
        printf("%34.100f, ", update_coeffs->band[r].ord_2.b2);
    }
    printf("*\n");

    for (int r = 0; r < 3; r++)
    {
        printf("%34.100f, ", update_coeffs->band[r].ord_2.a1);
    }
    printf("*\n");

    for (int r = 0; r < 3; r++)
    {
        printf("%34.100f, ", update_coeffs->band[r].ord_2.a2);
    }
    printf("*\n");
}
