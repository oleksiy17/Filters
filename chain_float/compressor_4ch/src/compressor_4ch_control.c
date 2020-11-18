#include <stdint.h>
#include <stddef.h>
#include "math.h"

#include "compressor_4ch_control.h"

/*******************************************************************************
 * Provides with the required data sizes for parameters and coefficients.
 *   It is caller responsibility to allocate enough memory (bytes) for them.
 * 
 * @param[out] params_bytes   required data size for storing parameters
 * @param[out] coeffs_bytes   required data size for storing coefficients
 * 
 * @return 0 if success, non-zero error code otherwise
 ******************************************************************************/
int32_t compressor_4ch_control_get_sizes(
    size_t*     params_bytes,
    size_t*     coeffs_bytes)
{
    *params_bytes = sizeof(comprssor_4ch_params);
    *coeffs_bytes = sizeof(compressor_4ch_coeffs);
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
int32_t compressor_4ch_control_initialize(
    void*       params,
    void*       coeffs,
    uint32_t    sample_rate)
{
    comprssor_4ch_params* init_params = (comprssor_4ch_params*)params;
    compressor_4ch_coeffs* init_coeffs = (compressor_4ch_coeffs*)coeffs;

    //  ch_1
    init_params->comp_ch_1_par.threshold = 0.0;
    init_params->comp_ch_1_par.ratio = 0.0;
    init_params->comp_ch_1_par.tauAttack = 0.0;
    init_params->comp_ch_1_par.tauRelease = 0.0;
    init_params->comp_ch_1_par.makeUpGain = 0.0;
    init_params->comp_ch_1_par.samplerate = 0.0;
    init_params->comp_ch_1_par.bpass = 0.0;

    init_coeffs->comp_ch_1_coef.threshold = 0.0;
    init_coeffs->comp_ch_1_coef.ratio = 0.0;
    init_coeffs->comp_ch_1_coef.alphaAttack = 0.0;
    init_coeffs->comp_ch_1_coef.alphaRelease = 0.0;
    init_coeffs->comp_ch_1_coef.makeUpGain = 0.0;
    init_coeffs->comp_ch_1_coef.samplerate = 0.0;
    init_coeffs->comp_ch_1_coef.bpass = 0.0;

    //  ch_2
    init_params->comp_ch_2_par.threshold = 0.0;
    init_params->comp_ch_2_par.ratio = 0.0;
    init_params->comp_ch_2_par.tauAttack = 0.0;
    init_params->comp_ch_2_par.tauRelease = 0.0;
    init_params->comp_ch_2_par.makeUpGain = 0.0;
    init_params->comp_ch_2_par.samplerate = 0.0;
    init_params->comp_ch_2_par.bpass = 0.0;

    init_coeffs->comp_ch_2_coef.threshold = 0.0;
    init_coeffs->comp_ch_2_coef.ratio = 0.0;
    init_coeffs->comp_ch_2_coef.alphaAttack = 0.0;
    init_coeffs->comp_ch_2_coef.alphaRelease = 0.0;
    init_coeffs->comp_ch_2_coef.makeUpGain = 0.0;
    init_coeffs->comp_ch_2_coef.samplerate = 0.0;
    init_coeffs->comp_ch_2_coef.bpass = 0.0;

    //  ch_3
    init_params->comp_ch_3_par.threshold = 0.0;
    init_params->comp_ch_3_par.ratio = 0.0;
    init_params->comp_ch_3_par.tauAttack = 0.0;
    init_params->comp_ch_3_par.tauRelease = 0.0;
    init_params->comp_ch_3_par.makeUpGain = 0.0;
    init_params->comp_ch_3_par.samplerate = 0.0;
    init_params->comp_ch_3_par.bpass = 0.0;

    init_coeffs->comp_ch_3_coef.threshold = 0.0;
    init_coeffs->comp_ch_3_coef.ratio = 0.0;
    init_coeffs->comp_ch_3_coef.alphaAttack = 0.0;
    init_coeffs->comp_ch_3_coef.alphaRelease = 0.0;
    init_coeffs->comp_ch_3_coef.makeUpGain = 0.0;
    init_coeffs->comp_ch_3_coef.samplerate = 0.0;
    init_coeffs->comp_ch_3_coef.bpass = 0.0;

    //  ch_4
    init_params->comp_ch_4_par.threshold = 0.0;
    init_params->comp_ch_4_par.ratio = 0.0;
    init_params->comp_ch_4_par.tauAttack = 0.0;
    init_params->comp_ch_4_par.tauRelease = 0.0;
    init_params->comp_ch_4_par.makeUpGain = 0.0;
    init_params->comp_ch_4_par.samplerate = 0.0;
    init_params->comp_ch_4_par.bpass = 0.0;

    init_coeffs->comp_ch_4_coef.threshold = 0.0;
    init_coeffs->comp_ch_4_coef.ratio = 0.0;
    init_coeffs->comp_ch_4_coef.alphaAttack = 0.0;
    init_coeffs->comp_ch_4_coef.alphaRelease = 0.0;
    init_coeffs->comp_ch_4_coef.makeUpGain = 0.0;
    init_coeffs->comp_ch_4_coef.samplerate = 0.0;
    init_coeffs->comp_ch_4_coef.bpass = 0.0;
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
int32_t compressor_4ch_set_parameter(
    void*       params,
    int32_t     id,
    float       value)
{
    comprssor_4ch_params* set_params = (comprssor_4ch_params*)params;

    if (id < 110)
    {
        switch (id) // ch_1
        {
            case 100:
            {
                set_params->comp_ch_1_par.threshold = value;
                break;
            }

            case 101:
            {
                set_params->comp_ch_1_par.ratio = value;
                break;
            }

            case 102:
            {
                set_params->comp_ch_1_par.tauAttack = value;
                break;
            }

            case 103:
            {
                set_params->comp_ch_1_par.tauRelease = value;
                break;
            }

            case 104:
            {
                set_params->comp_ch_1_par.makeUpGain = value;
                break;
            }

            case 105:
            {
                set_params->comp_ch_1_par.samplerate = value;
                break;
            }

            case 106:
            {
                set_params->comp_ch_1_par.tauEnvAtt = value;
                break;
            }

            case 107:
            {
                set_params->comp_ch_1_par.tauEnvRel = value;
                break;
            }

            case 108:
            {
                set_params->comp_ch_1_par.bpass = value;
                break;
            }

            default:
                break;
        }
    }
    else if(id < 120)
    {
        switch (id)     // ch_2
        {   
            case 110:
            {
                set_params->comp_ch_2_par.threshold = value;
                break;
            }

            case 111:
            {
                set_params->comp_ch_2_par.ratio = value;
                break;
            }

            case 112:
            {
                set_params->comp_ch_2_par.tauAttack = value;
                break;
            }

            case 113:
            {
                set_params->comp_ch_2_par.tauRelease = value;
                break;
            }

            case 114:
            {
                set_params->comp_ch_2_par.makeUpGain = value;
                break;
            }

            case 115:
            {
                set_params->comp_ch_2_par.samplerate = value;
                break;
            }

            case 116:
            {
                set_params->comp_ch_2_par.tauEnvAtt = value;
                break;
            }

            case 117:
            {
                set_params->comp_ch_2_par.tauEnvRel = value;
                break;
            }

            case 118:
            {
                set_params->comp_ch_2_par.bpass = value;
                break;
            }

            default:
                break;
        }
    }

    else if (id < 130)
    {
        switch (id)     // ch_3
        {
            case 120:
            {
                set_params->comp_ch_3_par.threshold = value;
                break;
            }
    
            case 121:
            {
                set_params->comp_ch_3_par.ratio = value;
                break;
            }
    
            case 122:
            {
                set_params->comp_ch_3_par.tauAttack = value;
                break;
            }
    
            case 123:
            {
                set_params->comp_ch_3_par.tauRelease = value;
                break;
            }
    
            case 124:
            {
                set_params->comp_ch_3_par.makeUpGain = value;
                break;
            }
    
            case 125:
            {
                set_params->comp_ch_3_par.samplerate = value;
                break;
            }
    
            case 126:
            {
                set_params->comp_ch_3_par.tauEnvAtt = value;
                break;
            }
    
            case 127:
            {
                set_params->comp_ch_3_par.tauEnvRel = value;
                break;
            }
    
            case 128:
            {
                set_params->comp_ch_3_par.bpass = value;
                break;
            }
    
            default:
                break;
        }
    }

    else if (id < 140)
    {
        switch (id)     // ch_4
        {
            case 130:
            {
                set_params->comp_ch_4_par.threshold = value;
                break;
            }
    
            case 131:
            {
                set_params->comp_ch_4_par.ratio = value;
                break;
            }
    
            case 132:
            {
                set_params->comp_ch_4_par.tauAttack = value;
                break;
            }
    
            case 133:
            {
                set_params->comp_ch_4_par.tauRelease = value;
                break;
            }
    
            case 134:
            {
                set_params->comp_ch_4_par.makeUpGain = value;
                break;
            }
    
            case 135:
            {
                set_params->comp_ch_4_par.samplerate = value;
                break;
            }
    
            case 136:
            {
                set_params->comp_ch_4_par.tauEnvAtt = value;
                break;
            }
    
            case 137:
            {
                set_params->comp_ch_4_par.tauEnvRel = value;
                break;
            }
    
            case 138:
            {
                set_params->comp_ch_4_par.bpass = value;
                break;
            }
    
            default:
                break;
        }
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
int32_t compressor_4ch_update_coeffs(
    void const* params,
    void*       coeffs)
{
    comprssor_4ch_params* update_params = (comprssor_4ch_params*)params;
    compressor_4ch_coeffs* update_coeffs = (compressor_4ch_coeffs*)coeffs;

    // CH_1
    update_coeffs->comp_ch_1_coef.threshold =  powf(10.0, (update_params->comp_ch_1_par.threshold/20.0));  //in linear
    update_coeffs->comp_ch_1_coef.ratio = update_params->comp_ch_1_par.ratio;

    update_coeffs->comp_ch_1_coef.alphaAttack = powf(M_e, (-(log(9)) / (0.001*update_params->comp_ch_1_par.tauAttack*update_params->comp_ch_1_par.samplerate)));
    update_coeffs->comp_ch_1_coef.alphaRelease = powf(M_e, (-(log(9)) / (0.001*update_params->comp_ch_1_par.tauRelease*update_params->comp_ch_1_par.samplerate)));

    update_coeffs->comp_ch_1_coef.attackEnv = powf(M_e, (-(log(9)) / (0.001*update_params->comp_ch_1_par.tauEnvAtt*update_params->comp_ch_1_par.samplerate)));
    update_coeffs->comp_ch_1_coef.releaseEnv = powf(M_e, (-(log(9)) / (0.001*update_params->comp_ch_1_par.tauEnvRel*update_params->comp_ch_1_par.samplerate)));

    update_coeffs->comp_ch_1_coef.makeUpGain = powf(10.0, (update_params->comp_ch_1_par.makeUpGain/20.0));
    update_coeffs->comp_ch_1_coef.samplerate = update_params->comp_ch_1_par.samplerate;


    // CH_2
    update_coeffs->comp_ch_2_coef.threshold = powf(10.0, (update_params->comp_ch_2_par.threshold / 20.0));  //in linear
    update_coeffs->comp_ch_2_coef.ratio = update_params->comp_ch_2_par.ratio;

    update_coeffs->comp_ch_2_coef.alphaAttack = powf(M_e, (-(log(9)) / (0.001*update_params->comp_ch_2_par.tauAttack*update_params->comp_ch_2_par.samplerate)));
    update_coeffs->comp_ch_2_coef.alphaRelease = powf(M_e, (-(log(9)) / (0.001*update_params->comp_ch_2_par.tauRelease*update_params->comp_ch_2_par.samplerate)));

    update_coeffs->comp_ch_2_coef.attackEnv = powf(M_e, (-(log(9)) / (0.001*update_params->comp_ch_2_par.tauEnvAtt*update_params->comp_ch_2_par.samplerate)));
    update_coeffs->comp_ch_2_coef.releaseEnv = powf(M_e, (-(log(9)) / (0.001*update_params->comp_ch_2_par.tauEnvRel*update_params->comp_ch_2_par.samplerate)));

    update_coeffs->comp_ch_2_coef.makeUpGain = powf(10.0, (update_params->comp_ch_2_par.makeUpGain / 20.0));
    update_coeffs->comp_ch_2_coef.samplerate = update_params->comp_ch_2_par.samplerate;


    // CH_3
    update_coeffs->comp_ch_3_coef.threshold = powf(10.0, (update_params->comp_ch_3_par.threshold / 20.0));  //in linear
    update_coeffs->comp_ch_3_coef.ratio = update_params->comp_ch_3_par.ratio;

    update_coeffs->comp_ch_3_coef.alphaAttack = powf(M_e, (-(log(9)) / (0.001*update_params->comp_ch_3_par.tauAttack*update_params->comp_ch_3_par.samplerate)));
    update_coeffs->comp_ch_3_coef.alphaRelease = powf(M_e, (-(log(9)) / (0.001*update_params->comp_ch_3_par.tauRelease*update_params->comp_ch_3_par.samplerate)));

    update_coeffs->comp_ch_3_coef.attackEnv = powf(M_e, (-(log(9)) / (0.001*update_params->comp_ch_3_par.tauEnvAtt*update_params->comp_ch_3_par.samplerate)));
    update_coeffs->comp_ch_3_coef.releaseEnv = powf(M_e, (-(log(9)) / (0.001*update_params->comp_ch_3_par.tauEnvRel*update_params->comp_ch_3_par.samplerate)));

    update_coeffs->comp_ch_3_coef.makeUpGain = powf(10.0, (update_params->comp_ch_3_par.makeUpGain / 20.0));
    update_coeffs->comp_ch_3_coef.samplerate = update_params->comp_ch_3_par.samplerate;


    // CH_4
    update_coeffs->comp_ch_4_coef.threshold = powf(10.0, (update_params->comp_ch_4_par.threshold / 20.0));  //in linear
    update_coeffs->comp_ch_4_coef.ratio = update_params->comp_ch_4_par.ratio;

    update_coeffs->comp_ch_4_coef.alphaAttack = powf(M_e, (-(log(9)) / (0.001*update_params->comp_ch_4_par.tauAttack*update_params->comp_ch_4_par.samplerate)));
    update_coeffs->comp_ch_4_coef.alphaRelease = powf(M_e, (-(log(9)) / (0.001*update_params->comp_ch_4_par.tauRelease*update_params->comp_ch_4_par.samplerate)));

    update_coeffs->comp_ch_4_coef.attackEnv = powf(M_e, (-(log(9)) / (0.001*update_params->comp_ch_4_par.tauEnvAtt*update_params->comp_ch_4_par.samplerate)));
    update_coeffs->comp_ch_4_coef.releaseEnv = powf(M_e, (-(log(9)) / (0.001*update_params->comp_ch_4_par.tauEnvRel*update_params->comp_ch_4_par.samplerate)));

    update_coeffs->comp_ch_4_coef.makeUpGain = powf(10.0, (update_params->comp_ch_4_par.makeUpGain / 20.0));
    update_coeffs->comp_ch_4_coef.samplerate = update_params->comp_ch_4_par.samplerate;
}
