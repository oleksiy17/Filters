 #include <stdint.h>
#include <stddef.h>
#include "fixedpoint.h"
//////////////////////////////////////
#include "compressor_4ch_control.h"
#include "compressor_4ch_process.h"

/*******************************************************************************
 * Provides with the required data sizes for states. It is caller responsibility
 *   to allocate enough memory (bytes) for them.
 * 
 * @param[out] states_bytes required data size for storing states
 * 
 * @return 0 if success, non-zero error code otherwise
 ******************************************************************************/
int32_t compressor_4ch_process_get_sizes(
    size_t*     states_bytes)
{
    *states_bytes = sizeof(compressor_4ch_states);
}


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
    void*       states)
{
    compressor_4ch_states* reset_states = (compressor_4ch_states*)states;

    reset_states->comp_ch_1_st.x = 0.0;
    reset_states->comp_ch_1_st.g_c = 0.0;
    reset_states->comp_ch_1_st.g_s = 0.0;
    reset_states->comp_ch_1_st.g_sPrev = 1.0;
    reset_states->comp_ch_1_st.g_m = 0.0;
    reset_states->comp_ch_1_st.envelope = 0.0;
    reset_states->comp_ch_1_st.envelope_prev = 0.0;

    reset_states->comp_ch_2_st.x = 0.0;
    reset_states->comp_ch_2_st.g_c = 0.0;
    reset_states->comp_ch_2_st.g_s = 0.0;
    reset_states->comp_ch_2_st.g_sPrev = 1.0;
    reset_states->comp_ch_2_st.g_m = 0.0;
    reset_states->comp_ch_2_st.envelope = 0.0;
    reset_states->comp_ch_2_st.envelope_prev = 0.0;

    reset_states->comp_ch_3_st.x = 0.0;
    reset_states->comp_ch_3_st.g_c = 0.0;
    reset_states->comp_ch_3_st.g_s = 0.0;
    reset_states->comp_ch_3_st.g_sPrev = 1.0;
    reset_states->comp_ch_3_st.g_m = 0.0;
    reset_states->comp_ch_3_st.envelope = 0.0;
    reset_states->comp_ch_3_st.envelope_prev = 0.0;

    reset_states->comp_ch_4_st.x = 0.0;
    reset_states->comp_ch_4_st.g_c = 0.0;
    reset_states->comp_ch_4_st.g_s = 0.0;
    reset_states->comp_ch_4_st.g_sPrev = 1.0;
    reset_states->comp_ch_4_st.g_m = 0.0;
    reset_states->comp_ch_4_st.envelope = 0.0;
    reset_states->comp_ch_4_st.envelope_prev = 0.0;

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
int32_t compressor_4ch_process(
    void const* coeffs,
    void*       states,
    void*       audio,
    size_t      samples_count)
{
    compressor_4ch_coeffs* coeffs_4ch_c;
    compressor_4ch_states* states_4ch_c;
    audio_buf_comp* audio_c;

    uint32_t i;

    coeffs_4ch_c = (compressor_4ch_coeffs*)coeffs;
    states_4ch_c = (compressor_4ch_states*)states;
    audio_c = (audio_buf_comp*)audio;

    //compressor_process((compressor_coeffs*)&coeffs_4ch_c->comp_ch_1_coef, (compressor_states*)&states_4ch_c->comp_ch_1_st, (tStereo_compr*)audio_c->cross_b.band_1, samples_count);

    if (coeffs_4ch_c->comp_ch_1_coef.bpass == 1)
    {
        compressor_process((compressor_coeffs*)&coeffs_4ch_c->comp_ch_1_coef, (compressor_states*)&states_4ch_c->comp_ch_1_st, (tStereo_compr*)audio_c->cross_b.band_1, samples_count);
    }

    if (coeffs_4ch_c->comp_ch_2_coef.bpass == 1)
    {
        compressor_process((compressor_coeffs*)&coeffs_4ch_c->comp_ch_2_coef, (compressor_states*)&states_4ch_c->comp_ch_2_st, (tStereo_compr*)audio_c->cross_b.band_2, samples_count);
    }
    
    if (coeffs_4ch_c->comp_ch_3_coef.bpass == 1)
    {
        compressor_process((compressor_coeffs*)&coeffs_4ch_c->comp_ch_3_coef, (compressor_states*)&states_4ch_c->comp_ch_3_st, (tStereo_compr*)audio_c->cross_b.band_3, samples_count);
    }

    if (coeffs_4ch_c->comp_ch_4_coef.bpass == 1)
    {
        compressor_process((compressor_coeffs*)&coeffs_4ch_c->comp_ch_4_coef, (compressor_states*)&states_4ch_c->comp_ch_4_st, (tStereo_compr*)audio_c->cross_b.band_4, samples_count);
    }
    

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
/*int32_t compressor_process(
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

        ((tStereo_compr*)audio_c)[i].L = mul_f( ((tStereo_compr*)audio_c)[i].L,  states_c->g_m);
        ((tStereo_compr*)audio_c)[i].R = mul_f( ((tStereo_compr*)audio_c)[i].R,  states_c->g_m);
    }
}*/

/*
xR_abs = fabsf(states_c->x.R);

        if (xR_abs > states_c->envelope_prev.R)              // comparison of current gain and previos gain
        {

            states_c->envelope.R = coeffs_c->attackEnv * states_c->envelope_prev.R + (1.0 - coeffs_c->attackEnv) * xR_abs;     // if current gain higher than previous -> attac
        }
        else
        {
            states_c->envelope.R = coeffs_c->releaseEnv * states_c->envelope_prev.R + (1.0 - coeffs_c->releaseEnv) * xR_abs;    // attenuate
        }

        states_c->envelope_prev.R = states_c->envelope.R;

        if (states_c->envelope.R < coeffs_c->threshold)
        {

            states_c->g_c.R = 1;
        }
        else
        {
            states_c->g_c.R = (coeffs_c->threshold * powf((states_c->envelope.R / coeffs_c->threshold), (1.0 / coeffs_c->ratio))) / states_c->envelope.R;
        }

        if (states_c->g_c.R <= states_c->g_sPrev.R)
        {
            states_c->g_s.R = coeffs_c->alphaAttack* states_c->g_sPrev.R + (1.0 - coeffs_c->alphaAttack)*states_c->g_c.R;
        }
        else
        {
            states_c->g_s.R = coeffs_c->alphaRelease* states_c->g_sPrev.R + (1.0 - coeffs_c->alphaRelease)*states_c->g_c.R;
        }

        states_c->g_sPrev.R = states_c->g_s.R;
        states_c->g_m.R = states_c->g_s.R * coeffs_c->makeUpGain;
*/