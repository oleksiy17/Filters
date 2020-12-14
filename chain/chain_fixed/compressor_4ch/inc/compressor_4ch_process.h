#ifndef __CROSSOVER_4CH_PROCESS_H__
#define __CROSSOVER_4CH_PROCESS_H__

#include <stdint.h>
#include <stddef.h>


/*******************************************************************************
 * Provides with the required data sizes for states. It is caller responsibility
 *   to allocate enough memory (bytes) for them.
 * 
 * @param[out] states_bytes required data size for storing states
 * 
 * @return 0 if success, non-zero error code otherwise
 ******************************************************************************/
int32_t compressor_4ch_process_get_sizes(
    size_t*     states_bytes);


/*******************************************************************************
 * Reset internal states. Configuration remains the same.
 * 
 * @param[in] coeffs        initialized coeffs
 * @param[in] states        initialized states
 * 
 * @return 0 on success, otherwise fail
 ******************************************************************************/
int32_t compressor_4ch_reset(
    void const* coeffs,
    void*       states);


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
int32_t compressor_4ch_process(
    void const* coeffs,
    void*       states,
    void*       audio,
    size_t      samples_count);


static __forceinline int32_t compressor_process(
    void const* coeffs,
    void*       states,
    void*       audio,
    size_t      samples_count)
{
    compressor_coeffs* coeffs_c;
    compressor_states* states_c;
    tStereo_compr* audio_c;
    uint32_t i;

    coeffs_c = (compressor_coeffs*)coeffs;
    states_c = (compressor_states*)states;
    audio_c = (tStereo_compr*)audio;

    float xL_abs;
    float xR_abs;
    float max_abs;

    for (i = 0; i < samples_count; i++)
    {
        xL_abs = abs_f(((tStereo_compr*)audio_c)[i].L);
        xR_abs = abs_f(((tStereo_compr*)audio_c)[i].R);

        max_abs = (xL_abs > xR_abs) ? xL_abs : xR_abs;

        if (max_abs >= states_c->envelope_prev)              // comparison of current gain and previos gain
        {
            max_abs = mul_f(max_abs, coeffs_c->_one_env_att);
            max_abs = mac_f(coeffs_c->attackEnv, states_c->envelope_prev, max_abs);
            states_c->envelope = max_abs;
        }
        else
        {
            max_abs = mul_f(max_abs, coeffs_c->_one_env_rel);
            max_abs = mac_f(coeffs_c->releaseEnv, states_c->envelope_prev, max_abs);
            states_c->envelope = max_abs;
        }

        states_c->envelope_prev = states_c->envelope;

        if (states_c->envelope < coeffs_c->threshold)
        {
            states_c->g_c = 1.0;
        }
        else
        {
            max_abs = div_f(coeffs_c->threshold, states_c->envelope);
            max_abs = pow_f(max_abs, coeffs_c->_rep_ratio);
            states_c->g_c = max_abs;
        }

        if (states_c->g_c <= states_c->g_sPrev)
        {
            max_abs = mul_f(states_c->g_c, coeffs_c->_one_g_att);
            max_abs = mac_f(coeffs_c->alphaAttack, states_c->g_sPrev, max_abs);
            states_c->g_s = max_abs;
        }
        else
        {
            max_abs = mul_f(states_c->g_c, coeffs_c->_one_g_rel);
            max_abs = mac_f(coeffs_c->alphaRelease, states_c->g_sPrev, max_abs);
            states_c->g_s = max_abs;
        }

        states_c->g_sPrev = states_c->g_s;
        states_c->g_m = mul_f(states_c->g_s, coeffs_c->makeUpGain);

        ((tStereo_compr*)audio_c)[i].L = mul_f(((tStereo_compr*)audio_c)[i].L, states_c->g_m);
        ((tStereo_compr*)audio_c)[i].R = mul_f(((tStereo_compr*)audio_c)[i].R, states_c->g_m);
    }
}
#endif