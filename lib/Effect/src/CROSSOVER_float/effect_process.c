#ifndef __EFFECT_PROCESS_H__
#define __EFFECT_PROCESS_H__

#include <stdint.h>
#include <stddef.h>
#include "process_fcns.h"
#include "crossover_datatypes.h"


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
    *states_bytes = sizeof(crossover_states);
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
    crossover_states* reset_states = (crossover_states*)states;

    reset_states->casc_1 = (apf_casc) { 
        { {0, 0},
          {0, 0},
          {0, 0} },

        { {0, 0},
          {0, 0},
          {0, 0} },

        { {0, 0},
          {0, 0},
          {0, 0} },
    };

    reset_states->casc_2_1 = (apf_casc) {
        { {0, 0},
          {0, 0},
          {0, 0} },

        { {0, 0},
          {0, 0},
          {0, 0} },

        { {0, 0},
          {0, 0},
          {0, 0} },
    };

    reset_states->casc_2_2 = (apf_casc) {
        { {0, 0},
          {0, 0},
          {0, 0} },

        { {0, 0},
          {0, 0},
          {0, 0} },

        { {0, 0},
          {0, 0},
          {0, 0} },
    };

    reset_states->lp_compens = (apf_unit) { 
        {0, 0},
        {0, 0},
        {0, 0}
    };

    reset_states->hp_compens = (apf_unit) {
        {0, 0},
        {0, 0},
        {0, 0}
    };
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
    crossover_coeffs* pr_coeffs;
    crossover_states* pr_states;
    tStereo* pr_audio;

    my_float xh;

    pr_coeffs = (crossover_coeffs*)coeffs;
    pr_states = (crossover_states*)states;
    pr_audio = (tStereo*)audio;

    pr_audio->L /= 2;
    pr_audio->R /= 2;
     

    for (my_uint32 i = 0; i < samples_count; i++)
    {
        // low-pass -3dB
        xh = mac_f(neg_f()



            

    }

}


#endif
