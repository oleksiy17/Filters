#include <stdint.h>
#include <stddef.h>

#include "equalizer_process.h"
#include "equalizer_control.h"



/*******************************************************************************
 * Provides with the required data sizes for states. It is caller responsibility
 *   to allocate enough memory (bytes) for them.
 * 
 * @param[out] states_bytes required data size for storing states
 * 
 * @return 0 if success, non-zero error code otherwise
 ******************************************************************************/
int32_t equalizer_process_get_sizes(
    size_t*     states_bytes)
{
    *states_bytes = sizeof(equalizer_states);
    return 0;
}


/*******************************************************************************
 * Reset internal states. Configuration remains the same.
 * 
 * @param[in] coeffs        initialized coeffs
 * @param[in] states        initialized states
 * 
 * @return 0 on success, otherwise fail
 ******************************************************************************/
int32_t equalizer_reset(
    void const* coeffs,
    void*       states)
{
    equalizer_coeffs *reset_coeffs = (equalizer_coeffs*)coeffs;
    equalizer_states *reset_states = (equalizer_states*)states;
    size_t i;
    
    for ( i = 0; i < (size_t)BANDS; i++)
    {
        biquad_reset(&reset_coeffs->bq[i], &reset_states->bq[i]);
    }

    return 0;    
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
int32_t equalizer_process(
    void const* coeffs,
    void*       states,
    void*       audio,
    size_t      samples_count)
{
    equalizer_coeffs *c = (equalizer_coeffs*)coeffs;
    equalizer_states *s = (equalizer_states*)states;
    
    if (!c->bypass)
    {
        for (size_t i = 0; i < BANDS; i++)
        {
            if (i == 4)
            {
                int k = 0;
            }
            biquad_process(&c->bq[i], &s->bq[i], audio, samples_count);
        }
    }
    
    return 0;
}



