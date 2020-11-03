#ifndef __EFFECT_PROCESS_H__
#define __EFFECT_PROCESS_H__

#include <stdint.h>
#include <stddef.h>
#include "fixedpoint.h"

#define X_MIN_VAL_THRESHOLD    0x800
#define MIN_DB                 0xC3CB43C0 
#define TWENTY_Q23             0xA000000
#define ONE_Q23                0x800000


typedef struct {
    my_sint32 threshold;
    my_sint32 ratio;

    my_sint32 alphaAttack;
    my_sint32 alphaRelease;

    my_sint32 makeUpGain;
    my_sint32 samplerate;
}compressor_coeffs;

typedef struct {
    my_sint32 L;
    my_sint32 R;
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

    reset_states->x.L = 0;
    reset_states->y.L = 0;
    reset_states->prev_y_dB.L = 0;
    reset_states->det_x_dB.L = 0;
    reset_states->det_y_dB.L = 0;
    reset_states->x_dB.L = 0;
    reset_states->y_dB.L = 0;
    reset_states->c_gain.L = 0;

    reset_states->x.R = 0;
    reset_states->y.R = 0;
    reset_states->prev_y_dB.R = 0;
    reset_states->det_x_dB.R = 0;
    reset_states->det_y_dB.R = 0;
    reset_states->x_dB.R = 0;
    reset_states->y_dB.R = 0;
    reset_states->c_gain.R = 0;
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

    my_sint32 axil_1;
    my_sint32 axil_2;

    for (i = 0; i < samples_count; i++)
    {
        axil_1 = 0;
        axil_2 = 0;

        states_c->x = ((tStereo*)audio_c)[i];

                            /*      Left channel   */    

        //ABS lin/log
        if (abs32(states_c->x.L) < X_MIN_VAL_THRESHOLD)                         // in < 1/1048576
        {
            states_c->x_dB.L = MIN_DB;                      // input signal AMP -> GAIN conversion -120,41198 Q8.23
        }
        else
        {
            states_c->x_dB.L = mul32_q(log10x(abs32(states_c->x.L)), (my_sint32)TWENTY_Q23, Q23);    // input signal AMP -> GAIN conversion       // x_dB in Q8.23
        }

        // -CT + CS
        if (states_c->x_dB.L > coeffs_c->threshold)
        {
            //states_c->y_dB.L = (1.0 - (1.0/coeffs_c->ratio))*(coeffs_c->threshold - states_c->x_dB.L);     //hard knee gain computition
            axil_1 = sub32(coeffs_c->threshold, states_c->x_dB.L);      // Q8.23
            axil_2 = div32_1_x(coeffs_c->ratio, Q23);                   // Q8.23
            axil_2 = sub32(ONE_Q23, axil_2);                           // Q8.23

            states_c->y_dB.L = mul32_q(axil_2, axil_1, Q23);            // Q8.23
        }
        else
        {
            states_c->y_dB.L = 0;                              
        }

        //states_c->c_gain.L = pow(10.0, ((coeffs_c->makeUpGain + states_c->y_dB.L) / 20));

        axil_1 = add32(coeffs_c->makeUpGain, states_c->y_dB.L);
        axil_2 = div32_1_x((my_sint32)TWENTY_Q23, Q23);
        axil_1 = mul32_q(axil_1, axil_2, Q23);
        states_c->c_gain.L = pow10x(axil_1);

        if (states_c->c_gain.L > states_c->prev_y_dB.L)
        {
            //states_c->det_x_dB.L = (1.0 - coeffs_c->alphaAttack)*states_c->c_gain.L + coeffs_c->alphaAttack*states_c->prev_y_dB.L;

            axil_1 = sub32(ONE_Q23, coeffs_c->alphaAttack);
            axil_1 = mul32_q(axil_1, states_c->c_gain.L, Q23);

            axil_2 = mul32_q(coeffs_c->alphaAttack, states_c->prev_y_dB.L, Q23);

            states_c->det_x_dB.L = add32(axil_1, axil_2);
        }
        else
        {
            //states_c->det_x_dB.L = (1.0 - coeffs_c->alphaRelease)*states_c->c_gain.L + coeffs_c->alphaRelease*states_c->prev_y_dB.L;

            axil_1 = sub32(ONE_Q23, coeffs_c->alphaRelease);
            axil_1 = mul32_q(axil_1, states_c->c_gain.L, Q23);

            axil_2 = mul32_q(coeffs_c->alphaRelease, states_c->prev_y_dB.L, Q23);

            states_c->det_x_dB.L = add32(axil_1, axil_2);
        }

        states_c->prev_y_dB.L = states_c->det_x_dB.L;

        states_c->y.L = mul32_q(states_c->x.L, states_c->det_x_dB.L, Q23);     // states_c->x.L * states_c->det_x_dB.L;//states_c->c_gain.R;
        ((tStereo*)audio_c)[i].L = states_c->y.L;

                             /*      Right channel       */

        if (abs32(states_c->x.R) < X_MIN_VAL_THRESHOLD)
        {
            states_c->x_dB.R = MIN_DB;
        }
        else
        {
            states_c->x_dB.R = states_c->x_dB.R = mul32_q(log10x(abs32(states_c->x.R)), TWENTY_Q23, Q23);
        }

        if (states_c->x_dB.R >= coeffs_c->threshold)
        {
            axil_1 = sub32(states_c->x_dB.R, coeffs_c->threshold);
            axil_2 = div32_1_x(coeffs_c->ratio, Q23);
            axil_1 = mul32_q(axil_1, axil_2, Q23);

            states_c->y_dB.R = add32(coeffs_c->threshold, axil_1);
        }
        else
        {
            states_c->y_dB.R = states_c->x_dB.R;
        }

        states_c->det_x_dB.R = sub32(states_c->x_dB.R, states_c->y_dB.R);

        if (states_c->det_x_dB.R > states_c->prev_y_dB.R)
        {
            axil_1 = sub32(ONE_Q23, coeffs_c->alphaAttack);
            axil_1 = mul32_q(axil_1, states_c->det_x_dB.R, Q23);

            axil_2 = mul32_q(coeffs_c->alphaAttack, states_c->prev_y_dB.R, Q23);
            
            states_c->det_y_dB.R = add32(axil_1, axil_2);
        }
        else
        {
            axil_1 = sub32(ONE_Q23, coeffs_c->alphaRelease);
            axil_1 = mul32_q(axil_1, states_c->det_x_dB.R, Q23);

            axil_2 = mul32_q(coeffs_c->alphaRelease, states_c->prev_y_dB.R, Q23);

            states_c->det_y_dB.R = add32(axil_1, axil_2);
        }

        axil_1 = sub32(coeffs_c->makeUpGain, states_c->det_y_dB.R);
        axil_2 = div32_1_x(TWENTY_Q23, Q23);

        states_c->c_gain.R = pow10x(axil_2);

        states_c->prev_y_dB.R = states_c->det_y_dB.R;

        states_c->y.R = mul32_q(states_c->x.R, states_c->c_gain.R, Q23);
        ((tStereo*)audio_c)[i].R = states_c->y.R;

        /********************************************************************/
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
            states_c->det_y_dB.R = coeffs_c->alphaRelease * states_c->prev_y_dB.R + (1.0 - coeffs_c->alphaRelease) * states_c->det_x_dB.R;    // attenuate
        }

        states_c->c_gain.R = pow(10.0, ((coeffs_c->makeUpGain - states_c->det_y_dB.R) / 20));


        states_c->prev_y_dB.R = states_c->det_y_dB.R;

        states_c->y.R = states_c->x.R * states_c->c_gain.R;//states_c->c_gain.R;
        ((tStereo*)audio_c)[i].R = states_c->y.R;
        
    }
}


#endif
