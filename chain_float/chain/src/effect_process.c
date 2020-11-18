#include <stdint.h>
#include <stddef.h>

#include "effect_process.h"
#include "effect_control.h"


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
    *states_bytes = sizeof(chain_states);
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
    chain_coeffs* reset_coeffs = (chain_coeffs*)coeffs;
    chain_states* resrt_states = (chain_states*)states;

    compressor_4ch_reset((compressor_4ch_coeffs*)&reset_coeffs->compr_4ch_coef, (compressor_4ch_states*)&resrt_states->compr_4ch_st);
    crossover_reset((crossover_coeffs*)&reset_coeffs->cross_coef, (crossover_states*)&resrt_states->cross_st);
    equalizer_reset((equalizer_coeffs*)&reset_coeffs->equal_coef , (equalizer_states*)&resrt_states->equal_st);
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
    chain_coeffs* process_coeffs = (chain_coeffs*)coeffs;
    chain_states* process_states = (chain_states*)states;
    stereo* in = (stereo*)audio;

    size_t i;

    for (i = 0; i < samples_count; i++)
    {
        equalizer_process((equalizer_coeffs*)&process_coeffs->equal_coef, (equalizer_states*)&process_states->equal_st, audio, samples_count);

    }

}
