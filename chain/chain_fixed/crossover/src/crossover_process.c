#include <stdint.h>
#include <stddef.h>

#include "crossover_control.h"
#include "crossover_process.h"


/*******************************************************************************
 * Provides with the required data sizes for states. It is caller responsibility
 *   to allocate enough memory (bytes) for them.
 * 
 * @param[out] states_bytes required data size for storing states
 * 
 * @return 0 if success, non-zero error code otherwise
 ******************************************************************************/
int32_t crossover_process_get_sizes(
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
int32_t crossover_reset(
    void const* coeffs,
    void*       states)
{ 
    crossover_states* reset_states = (crossover_states*)states;
    
    reset_states->x0_ord1 = (crosStereo) { 0, 0 };
    reset_states->x0_ord2 = (crosStereo) { 0, 0 };
    reset_states->y0_ord1 = (crosStereo) { 0, 0 };
    reset_states->y0_ord2 = (crosStereo) { 0, 0 };

    reset_states->z_x1_ord1 = (crosStereo) { 0, 0 };
    reset_states->z_x1_ord2 = (crosStereo) { 0, 0 };
    reset_states->z_x2_ord2 = (crosStereo) { 0, 0 };
    reset_states->z_y1_ord1 = (crosStereo) { 0, 0 };
    reset_states->z_y1_ord2 = (crosStereo) { 0, 0 };
    reset_states->z_y2_ord2 = (crosStereo) { 0, 0 };
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
int32_t crossover_process(
    void const* coeffs,
    void*       states,
    void*       audio,
    size_t      samples_count)
{
    crossover_coeffs* cros_coeffs = (crossover_coeffs*)coeffs;
    crossover_states* cros_states = (crossover_states*)states;



}

