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

    float attackEnv;
    float releaseEnv;

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
    tStereo x_sc;       //

    tStereo g_c;

    tStereo g_s;
    tStereo g_sPrev;

    tStereo g_m;

    tStereo g_lin;

    tStereo envelope;
    tStereo envelope_prev;
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
    reset_states->x_dB.L = 0.0;
    reset_states->x_sc.L = 0.0;
    reset_states->g_c.L = 0.0;
    reset_states->g_s.L = 0.0;
    reset_states->g_sPrev.L = 0.0;
    reset_states->g_m.L = 0.0;
    reset_states->g_lin.L = 0.0;
    reset_states->envelope.L = 0.0;
    reset_states->envelope_prev.L = 0.0;

    reset_states->x.R = 0.0;
    reset_states->y.R = 0.0;
    reset_states->x_dB.R = 0.0;
    reset_states->x_sc.R = 0.0;
    reset_states->g_c.R = 0.0;
    reset_states->g_s.R = 0.0;
    reset_states->g_sPrev.R = 0.0;
    reset_states->g_m.R = 0.0;
    reset_states->g_lin.R = 0.0;
    reset_states->envelope.R = 0.0;
    reset_states->envelope_prev.R = 0.0;


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
    float x_abs;
    float axil2;

    for (i = 0; i < samples_count; i++)
    {
        states_c->x = ((tStereo*)audio_c)[i];

        /*axil1 = fabsf(states_c->x.L);
        if (axil1 > states_c->envelope_prev.L)              // comparison of current gain and previos gain
        {
            
            states_c->envelope.L = coeffs_c->attackEnv * states_c->envelope_prev.L + (1.0 - coeffs_c->attackEnv) * axil1;     // if current gain higher than previous -> attac
        }
        else
        {
            states_c->envelope.L = coeffs_c->releaseEnv * states_c->envelope_prev.L + (1.0 - coeffs_c->releaseEnv) * axil1;    // attenuate
        }

        states_c->envelope_prev.L = states_c->envelope.L;

        //      dB
        if (states_c->envelope.L <= 0.000001)
        {
            states_c->x_dB.L = -120.0;
        }
        else
        {
            states_c->x_dB.L = 20 * log10f(states_c->envelope.L);
        }
        

        //          Gain Computer

        if (states_c->x_dB.L < coeffs_c->threshold)
        {
            states_c->x_sc.L = states_c->x_dB.L;
        }
        else
        {
            states_c->x_sc.L = coeffs_c->threshold + ((states_c->x_dB.L - coeffs_c->threshold) / coeffs_c->ratio);
        }

        states_c->g_c.L = states_c->x_sc.L - states_c->x_dB.L;

        if (states_c->g_c.L <= states_c->g_sPrev.L)
        {
            states_c->g_s.L = coeffs_c->alphaAttack* states_c->g_sPrev.L + (1.0 - coeffs_c->alphaAttack)*states_c->g_c.L;
        }
        else
        {
            states_c->g_s.L = coeffs_c->alphaRelease* states_c->g_sPrev.L + (1.0 - coeffs_c->alphaRelease)*states_c->g_c.L;
        }

        states_c->g_sPrev.L = states_c->g_s.L;

        states_c->g_m.L = states_c->g_s.L + coeffs_c->makeUpGain;

        states_c->g_lin.L = powf(10.0, (states_c->g_m.L / 20.0));
        */

        // envelope of the signal
        x_abs = fabsf(states_c->x.L);

        if (x_abs > states_c->envelope_prev.L)              // comparison of current gain and previos gain
        {

            states_c->envelope.L = coeffs_c->attackEnv * states_c->envelope_prev.L + (1.0 - coeffs_c->attackEnv) * x_abs;     // if current gain higher than previous -> attac
        }
        else
        {
            states_c->envelope.L = coeffs_c->releaseEnv * states_c->envelope_prev.L + (1.0 - coeffs_c->releaseEnv) * x_abs;    // attenuate
        }

        states_c->envelope_prev.L = states_c->envelope.L;

        if (states_c->envelope.L < coeffs_c->threshold)
        {
            //states_c->x_sc.L = states_c->envelope.L;
            states_c->g_c.L = 1;
        }
        else
        {
            states_c->x_sc.L = coeffs_c->threshold + ((states_c->envelope.L - coeffs_c->threshold) / coeffs_c->ratio);
            states_c->g_c.L = states_c->x_sc.L / states_c->envelope.L;
        }

        if (states_c->g_c.L <= states_c->g_sPrev.L)
        {
            states_c->g_s.L = coeffs_c->alphaAttack* states_c->g_sPrev.L + (1.0 - coeffs_c->alphaAttack)*states_c->g_c.L;
        }
        else
        {
            states_c->g_s.L = coeffs_c->alphaRelease* states_c->g_sPrev.L + (1.0 - coeffs_c->alphaRelease)*states_c->g_c.L;
        }

        states_c->g_sPrev.L = states_c->g_s.L;

        states_c->g_m.L = states_c->g_s.L * coeffs_c->makeUpGain;


        ((tStereo*)audio_c)[i].L = states_c->x.L * states_c->g_m.L;
        ((tStereo*)audio_c)[i].R = states_c->envelope.L;
    }
}


#endif
