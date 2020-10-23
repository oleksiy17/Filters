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
    tStereo in_amp;             //
    tStereo out_amp;

    tStereo in_dB;        //
    tStereo out_dB;       //

    tStereo prev_out_dB;

    tStereo delta_in_db;
    tStereo delta_out_db;

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

    reset_states->in_amp.L = 0.0;
    reset_states->out_amp.L = 0.0;
    reset_states->prev_out_dB.L = 0.0;
    reset_states->delta_in_db.L = 0.0;
    reset_states->delta_out_db.L = 0.0;
    reset_states->in_dB.L = 0.0;
    reset_states->out_dB.L = 0.0;
    reset_states->c_gain.L = 0.0;

    reset_states->in_amp.R = 0.0;
    reset_states->out_amp.R = 0.0;
    reset_states->prev_out_dB.R = 0.0;
    reset_states->delta_in_db.R = 0.0;
    reset_states->delta_out_db.R = 0.0;
    reset_states->in_dB.R = 0.0;
    reset_states->out_dB.R = 0.0;
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

    for (i = 0; i < samples_count; i++)
    {
        states_c->in_amp = ((tStereo*)audio_c)[i];

                            /*      Left channel       */

        if (fabsf(states_c->in_amp.L) < 0.000001)
        {
            states_c->in_dB.L = -120;                                       // input signal AMP -> GAIN conversion
        }
        else
        {
            states_c->in_dB.L = 20 * log10f(fabsf(states_c->in_amp.L));   // input signal AMP -> GAIN conversion
        }

        if (states_c->in_dB.L >= coeffs_c->threshold)
        {
            states_c->out_dB.L = coeffs_c->threshold + ((states_c->in_dB.L - coeffs_c->threshold) / coeffs_c->ratio);     //hard knee gain computition
        }
        else
        {
            states_c->out_dB.L = states_c->in_dB.L;
        }

        states_c->delta_in_db.L = states_c->in_dB.L - states_c->out_dB.L;

        if (states_c->delta_in_db.L > states_c->prev_out_dB.L)
        {
            states_c->delta_out_db.L = (coeffs_c->alphaAttack * states_c->prev_out_dB.L) + (1.0 - coeffs_c->alphaAttack) * states_c->delta_in_db.L;
        }
        else
        {
            states_c->delta_out_db.L = coeffs_c->alphaRelease * states_c->prev_out_dB.L + (1.0 - coeffs_c->alphaRelease) * states_c->delta_in_db.L;
        }


        states_c->c_gain.L = pow(10.0, ((coeffs_c->makeUpGain - states_c->delta_out_db.L) / 20));
        states_c->prev_out_dB.L = states_c->delta_out_db.L;

        states_c->out_amp.L = states_c->in_amp.L*states_c->c_gain.L;
        ((tStereo*)audio_c)[i].L = states_c->out_amp.L;

                             /*      Right channel       */


        if (fabsf(states_c->in_amp.R) < 0.000001)
        {
            states_c->in_dB.R = -120;                                       // input signal AMP -> GAIN conversion
        }
        else
        {
            states_c->in_dB.R = 20 * log10f(fabsf(states_c->in_amp.R));   // input signal AMP -> GAIN conversion
        }

        if (states_c->in_dB.R >= coeffs_c->threshold)
        {
            states_c->out_dB.R = coeffs_c->threshold + ((states_c->in_dB.R - coeffs_c->threshold) / coeffs_c->ratio);     //hard knee gain computition
        }
        else
        {
            states_c->out_dB.R = states_c->in_dB.R;                         // if threshold is not reached               
        }

        states_c->delta_in_db.R = states_c->in_dB.R - states_c->out_dB.R;   // gain difference out - in

        if (states_c->delta_in_db.R > states_c->prev_out_dB.R)              // comparison of current gain and previos gain
        {
            states_c->delta_out_db.R = (coeffs_c->alphaAttack * states_c->prev_out_dB.R) + (1.0 - coeffs_c->alphaAttack) * states_c->delta_in_db.R;     // if current gain higher than previous -> attac
        }
        else
        {
            states_c->delta_out_db.R = coeffs_c->alphaRelease * states_c->prev_out_dB.R + (1.0 - coeffs_c->alphaRelease) * states_c->delta_in_db.R;     // attenuate
        }


        states_c->c_gain.R = pow(10.0, ((coeffs_c->makeUpGain - states_c->delta_out_db.R) / 20));
        states_c->prev_out_dB.R = states_c->delta_out_db.R;

        states_c->out_amp.R = states_c->in_amp.R * states_c->c_gain.R;
        ((tStereo*)audio_c)[i].R = states_c->out_amp.R;

    }
}


#endif
