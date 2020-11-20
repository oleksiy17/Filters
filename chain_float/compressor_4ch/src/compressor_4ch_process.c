 #include <stdint.h>
#include <stddef.h>
#include "fixedpoint.h"

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

    reset_states->comp_ch_1_st.x.L = 0.0;
    reset_states->comp_ch_1_st.g_c.L = 0.0;
    reset_states->comp_ch_1_st.g_s.L = 0.0;
    reset_states->comp_ch_1_st.g_sPrev.L = 1.0;
    reset_states->comp_ch_1_st.g_m.L = 0.0;
    reset_states->comp_ch_1_st.envelope.L = 0.0;
    reset_states->comp_ch_1_st.envelope_prev.L = 0.0;

    reset_states->comp_ch_1_st.x.R = 0.0;
    reset_states->comp_ch_1_st.g_c.R = 0.0;
    reset_states->comp_ch_1_st.g_s.R = 0.0;
    reset_states->comp_ch_1_st.g_sPrev.R = 1.0;
    reset_states->comp_ch_1_st.g_m.R = 0.0;
    reset_states->comp_ch_1_st.envelope.R = 0.0;
    reset_states->comp_ch_1_st.envelope_prev.R = 0.0;

    reset_states->comp_ch_2_st.x.L = 0.0;
    reset_states->comp_ch_2_st.g_c.L = 0.0;
    reset_states->comp_ch_2_st.g_s.L = 0.0;
    reset_states->comp_ch_2_st.g_sPrev.L = 1.0;
    reset_states->comp_ch_2_st.g_m.L = 0.0;
    reset_states->comp_ch_2_st.envelope.L = 0.0;
    reset_states->comp_ch_2_st.envelope_prev.L = 0.0;

    reset_states->comp_ch_2_st.x.R = 0.0;
    reset_states->comp_ch_2_st.g_c.R = 0.0;
    reset_states->comp_ch_2_st.g_s.R = 0.0;
    reset_states->comp_ch_2_st.g_sPrev.R = 1.0;
    reset_states->comp_ch_2_st.g_m.R = 0.0;
    reset_states->comp_ch_2_st.envelope.R = 0.0;
    reset_states->comp_ch_2_st.envelope_prev.R = 0.0;

    reset_states->comp_ch_3_st.x.L = 0.0;
    reset_states->comp_ch_3_st.g_c.L = 0.0;
    reset_states->comp_ch_3_st.g_s.L = 0.0;
    reset_states->comp_ch_3_st.g_sPrev.L = 1.0;
    reset_states->comp_ch_3_st.g_m.L = 0.0;
    reset_states->comp_ch_3_st.envelope.L = 0.0;
    reset_states->comp_ch_3_st.envelope_prev.L = 0.0;

    reset_states->comp_ch_3_st.x.R = 0.0;
    reset_states->comp_ch_3_st.g_c.R = 0.0;
    reset_states->comp_ch_3_st.g_s.R = 0.0;
    reset_states->comp_ch_3_st.g_sPrev.R = 1.0;
    reset_states->comp_ch_3_st.g_m.R = 0.0;
    reset_states->comp_ch_3_st.envelope.R = 0.0;
    reset_states->comp_ch_3_st.envelope_prev.R = 0.0;

    reset_states->comp_ch_4_st.x.L = 0.0;
    reset_states->comp_ch_4_st.g_c.L = 0.0;
    reset_states->comp_ch_4_st.g_s.L = 0.0;
    reset_states->comp_ch_4_st.g_sPrev.L = 1.0;
    reset_states->comp_ch_4_st.g_m.L = 0.0;
    reset_states->comp_ch_4_st.envelope.L = 0.0;
    reset_states->comp_ch_4_st.envelope_prev.L = 0.0;

    reset_states->comp_ch_4_st.x.R = 0.0;
    reset_states->comp_ch_4_st.g_c.R = 0.0;
    reset_states->comp_ch_4_st.g_s.R = 0.0;
    reset_states->comp_ch_4_st.g_sPrev.R = 1.0;
    reset_states->comp_ch_4_st.g_m.R = 0.0;
    reset_states->comp_ch_4_st.envelope.R = 0.0;
    reset_states->comp_ch_4_st.envelope_prev.R = 0.0;
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

    if (coeffs_4ch_c->comp_ch_1_coef.bpass == 0)
    {
        compressor_process((compressor_coeffs*)&coeffs_4ch_c->comp_ch_1_coef, (compressor_states*)&states_4ch_c->comp_ch_1_st, (audio_buf_comp*)audio_c->band_4, samples_count);
    }

    if (coeffs_4ch_c->comp_ch_2_coef.bpass == 0)
    {
        compressor_process((compressor_coeffs*)&coeffs_4ch_c->comp_ch_2_coef, (compressor_states*)&states_4ch_c->comp_ch_2_st, (audio_buf_comp*)audio_c->band_2, samples_count);
    }
    
    if (coeffs_4ch_c->comp_ch_3_coef.bpass == 0)
    {
        compressor_process((compressor_coeffs*)&coeffs_4ch_c->comp_ch_3_coef, (compressor_states*)&states_4ch_c->comp_ch_3_st, (audio_buf_comp*)audio_c->band_3, samples_count);
    }

    if (coeffs_4ch_c->comp_ch_4_coef.bpass == 0)
    {
        compressor_process((compressor_coeffs*)&coeffs_4ch_c->comp_ch_4_coef, (compressor_states*)&states_4ch_c->comp_ch_4_st, (audio_buf_comp*)audio_c->band_4, samples_count);
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
int32_t compressor_process(
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


    for (i = 0; i < samples_count; i++)
    {
        states_c->x = ((tStereo_compr*)audio_c)[i];

        xL_abs = fabsf(states_c->x.L);

        if (xL_abs > states_c->envelope_prev.L)              // comparison of current gain and previos gain
        {

            states_c->envelope.L = coeffs_c->attackEnv * states_c->envelope_prev.L + (1.0 - coeffs_c->attackEnv) * xL_abs;     // if current gain higher than previous -> attac
        }
        else
        {
            states_c->envelope.L = coeffs_c->releaseEnv * states_c->envelope_prev.L + (1.0 - coeffs_c->releaseEnv) * xL_abs;    // attenuate
        }

        states_c->envelope_prev.L = states_c->envelope.L;

        if (states_c->envelope.L < coeffs_c->threshold)
        { 
            states_c->g_c.L = 1;
        }
        else
        {
            states_c->g_c.L = (coeffs_c->threshold * powf((states_c->envelope.L / coeffs_c->threshold), (1.0 / coeffs_c->ratio))) / states_c->envelope.L;
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

        ((tStereo_compr*)audio_c)[i].L = states_c->x.L * states_c->g_m.L;
        ((tStereo_compr*)audio_c)[i].R = states_c->x.R * states_c->g_m.R;
    }
}
