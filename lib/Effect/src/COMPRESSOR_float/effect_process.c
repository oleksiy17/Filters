#ifndef __EFFECT_PROCESS_H__
#define __EFFECT_PROCESS_H__

#include <stdint.h>
#include <stddef.h>
#include "fixedpoint.h"

typedef struct {
    float threshold;
    float ratio;

    float alphaAttack;
    float alphaRelease;

    float makeUpGain;
    float samplerate;
}compressor_coeffs;

typedef struct {
    float L;
    float R;
}tStereo;

typedef struct {
    tStereo x;             //
    tStereo y;

    tStereo x_dB;        //
    tStereo y_dB;       //

    tStereo prev_y_dB;

    tStereo det_x_dB;
    tStereo det_y_dB;

    tStereo c_gain;
}compressor_states;



/*******************************************************************************
 * Provides with the required data sizes for states. It is caller responsibility
 *   to allocate enough memory (bytes) for them.
 * 
 * @param[out] states_bytes required data size for storing states
 * 
 * @return 0 if success, non-zero error code otherwise
 ******************************************************************************/
int32_t effect_process_get_sizes(
    size_t*     states_bytes)
{
    *states_bytes = sizeof(compressor_states);
}


/*******************************************************************************
 * Reset internal states. Configuration remains the same.
 * 
 * @param[in] coeffs        initialized coeffs
 * @param[in] states        initialized states
 * 
 * @return 0 on success, otherwise fail
 ******************************************************************************/
int32_t effect_reset(
    void const* coeffs,
    void*       states)
{
    compressor_states* reset_states = (compressor_states*)states;

    reset_states->x.L = 0.0;
    reset_states->y.L = 0.0;
    reset_states->prev_y_dB.L = 0.0;
    reset_states->det_x_dB.L = 0.0;
    reset_states->det_y_dB.L = 0.0;
    reset_states->x_dB.L = 0.0;
    reset_states->y_dB.L = 0.0;
    reset_states->c_gain.L = 0.0;

    reset_states->x.R = 0.0;
    reset_states->y.R = 0.0;
    reset_states->prev_y_dB.R = 0.0;
    reset_states->det_x_dB.R = 0.0;
    reset_states->det_y_dB.R = 0.0;
    reset_states->x_dB.R = 0.0;
    reset_states->y_dB.R = 0.0;
    reset_states->c_gain.R = 0.0;
}


/*******************************************************************************
 * Process all available data in the input audio buffer (in-place processing).
 *   Samples are presented in the interleaved manner [L, R, L, R, ...].
 * 
 * @param[in] coeffs        initialized coeffs
 * @param[in] states        initialized states
 * @param[in,out] audio     array of input samples in the interleaved manner
 * @param[in] samples_count count of multichannel samples (L, R pairs) in audio
 * 
 * @return 0 on success, otherwise fail
 ******************************************************************************/
int32_t effect_process(
    void const* coeffs,
    void*       states,
    void*       audio,
    size_t      samples_count)
{
    compressor_coeffs* coeffs_c;
    compressor_states* states_c;
    tStereo* audio_c;
    uint32_t i;

    coeffs_c = (compressor_coeffs*)coeffs;
    states_c = (compressor_states*)states;
    audio_c = (tStereo*)audio;
    float axil1;
    float axil2;

    for (i = 0; i < samples_count; i++)
    {
        states_c->x = ((tStereo*)audio_c)[i];

                            /*      Left channel       */

        //ABS lin/log
        if (fabsf(states_c->x.L) < 0.000001)
        {
            states_c->x_dB.L = -120;                                       // input signal AMP -> GAIN conversion
        }
        else
        {
            states_c->x_dB.L = 20 * log10f(fabsf(states_c->x.L));   // input signal AMP -> GAIN conversion
        }

        // -CT + CS
        if (states_c->x_dB.L > coeffs_c->threshold)
        {
            states_c->y_dB.L = (1.0 - (1.0/coeffs_c->ratio))*(coeffs_c->threshold - states_c->x_dB.L);     //hard knee gain computition
        }
        else
        {
            states_c->y_dB.L = 0;                         // if threshold is not reached               
        }

        states_c->c_gain.L = pow(10.0, ((coeffs_c->makeUpGain + states_c->y_dB.L) / 20));

        if (states_c->c_gain.L > states_c->prev_y_dB.L)
        {
            axil1 = 1.0 - coeffs_c->alphaAttack;
            axil1 *= states_c->c_gain.L;
            axil2 = coeffs_c->alphaAttack*states_c->prev_y_dB.L;
            states_c->det_x_dB.L = axil1 + axil2;
            //states_c->det_x_dB.L = (1.0 - coeffs_c->alphaAttack)*states_c->c_gain.L + coeffs_c->alphaAttack*states_c->prev_y_dB.L;
        }
        else
        {
            axil1 = 1.0 - coeffs_c->alphaAttack;
            axil1 *= states_c->c_gain.L;
            axil2 = coeffs_c->alphaAttack*states_c->prev_y_dB.L;
            states_c->det_x_dB.L = axil1 + axil2;

            //states_c->det_x_dB.L = (1.0 - coeffs_c->alphaRelease)*states_c->c_gain.L + coeffs_c->alphaRelease*states_c->prev_y_dB.L;
        }

        states_c->prev_y_dB.L = states_c->det_x_dB.L;

        states_c->y.L = states_c->x.L * states_c->det_x_dB.L;//states_c->c_gain.R;
        ((tStereo*)audio_c)[i].L = states_c->y.L;
       
        /*if (fabsf(states_c->x.L) < 0.000001)
        {
            states_c->x_dB.L = -120;                                       // input signal AMP -> GAIN conversion
        } else
            {
             states_c->x_dB.L = 20 * log10f(fabsf(states_c->x.L));   // input signal AMP -> GAIN conversion
            }

        if (states_c->x_dB.L > coeffs_c->threshold)
        {
            states_c->y_dB.L = coeffs_c->threshold + ((states_c->x_dB.L - coeffs_c->threshold) / coeffs_c->ratio);     //hard knee gain computition
        } else
            {
                states_c->y_dB.L = states_c->x_dB.L;                         // if threshold is not reached               
            }

        states_c->det_x_dB.L = states_c->y_dB.L - states_c->x_dB.L;
        states_c->c_gain.L = pow(10.0, ((coeffs_c->makeUpGain + states_c->det_x_dB.L) / 20));

        if (states_c->c_gain.L > states_c->prev_y_dB.L)
        {
            states_c->det_y_dB.L = (1.0 - coeffs_c->alphaAttack)*states_c->c_gain.L + coeffs_c->alphaAttack*states_c->prev_y_dB.L;
        } else
            {
                states_c->det_y_dB.L = (1.0 - coeffs_c->alphaRelease)*states_c->c_gain.L + coeffs_c->alphaRelease*states_c->prev_y_dB.L;
            }

        states_c->prev_y_dB.L = states_c->det_y_dB.L;
         
        ((tStereo*)audio_c)[i].L = states_c->x.L * states_c->det_y_dB.L;*/

                             /*      Right channel       */

        if (fabsf(states_c->x.R) < 0.000001)
        {
            states_c->x_dB.R = -120;                                       // input signal AMP -> GAIN conversion
        }
        else
        {
            states_c->x_dB.R = 20 * log10f(fabsf(states_c->x.R));   // input signal AMP -> GAIN conversion
        }

        if (states_c->x_dB.R >= coeffs_c->threshold)
        {
            states_c->y_dB.R = coeffs_c->threshold + ((states_c->x_dB.R - coeffs_c->threshold) / coeffs_c->ratio);     //hard knee gain computition
        }
        else
        {
            states_c->y_dB.R = states_c->x_dB.R;                         // if threshold is not reached               
        }

        states_c->det_x_dB.R = states_c->x_dB.R - states_c->y_dB.R;   // gain difference out - in
        

        if (states_c->det_x_dB.R > states_c->prev_y_dB.R)              // comparison of current gain and previos gain
        {
            states_c->det_y_dB.R = coeffs_c->alphaAttack * states_c->prev_y_dB.R + (1.0 - coeffs_c->alphaAttack) * states_c->det_x_dB.R;     // if current gain higher than previous -> attac
        }
        else
        {
            states_c->det_y_dB.R =coeffs_c->alphaRelease * states_c->prev_y_dB.R + (1.0 - coeffs_c->alphaRelease) * states_c->det_x_dB.R;    // attenuate
        }

        states_c->c_gain.R = pow(10.0, ((coeffs_c->makeUpGain - states_c->det_y_dB.R) / 20));


        states_c->prev_y_dB.R = states_c->det_y_dB.R;

        states_c->y.R = states_c->x.R * states_c->c_gain.R;//states_c->c_gain.R;
        ((tStereo*)audio_c)[i].R = states_c->y.R;

    }
}


#endif
