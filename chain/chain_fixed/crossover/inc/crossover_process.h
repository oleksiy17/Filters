#ifndef __CROSSOVER_PROCESS_H__
#define __CROSSOVER_PROCESS_H__

#include <stdint.h>
#include <stddef.h>

#include "crossover_control.h"

/*******************************************************************************
 * Provides with the required data sizes for states. It is caller responsibility
 *   to allocate enough memory (bytes) for them.
 * 
 * @param[out] states_bytes required data size for storing states
 * 
 * @return 0 if success, non-zero error code otherwise
 ******************************************************************************/
int32_t crossover_process_get_sizes(
    size_t*     states_bytes);


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
int32_t crossover_process(
    void const* coeffs,
    void*       states,
    void*       audio,
    size_t      samples_count);



static __forceinline crosStereo order_1(crosStereo *in, crossover_coeffs *coeffs, crossover_states *states)
{
    my_sint64 acc;

    states->x0_ord1.L = rsh32(in->L, 1); // Q30

    acc = 0;
    acc = mac64(states->x0_ord1.L, coeffs->k0, acc);         // acc Q2.61
    acc = msub64(states->z_y1_ord1.L, coeffs->k0, acc); //acc Q 2.61
    acc = add64(acc, lsh64(states->z_x1_ord1.L, 31));   // acc Q 2.61
    acc = lsh64(acc, 1);  // result Q1.62
   
    states->z_x1_ord1.L = states->x0_ord1.L;
    states->z_y1_ord1.L = (my_sint32)(acc >> 32);
    states->y0_ord1.L = lsh32(states->z_y1_ord1.L, 1);

    ////////////////////////////////////////////////////////////

    states->x0_ord1.R = rsh32(in->R, 1); //Q30

    acc = 0;
    acc = mac64(states->x0_ord1.R, coeffs->k0, acc);         // acc Q2.61
    acc = msub64(states->z_y1_ord1.R, coeffs->k0, acc); //acc Q 2.61
    acc = add64(acc, lsh64(states->z_x1_ord1.R, 31));   // acc Q 2.61
    acc = lsh64(acc, 1);  // result Q1.62

    states->z_x1_ord1.R = states->x0_ord1.R;
    states->z_y1_ord1.R = (my_sint32)(acc >> 32);
    states->y0_ord1.R = lsh32(states->z_y1_ord1.R, 1);

    return states->y0_ord1;
}

static __forceinline crosStereo order_2(crosStereo *in, crossover_coeffs *coeffs, crossover_states *states)
{
    my_sint32 acc;

    states->x0_ord2.L = rsh32(in->L, 1);

    acc = 0;
    acc = mac64(states->x0_ord2.L, coeffs->k2, acc);
    acc = msub64(states->z_y2_ord2.L, coeffs->k2, acc);
    acc = mac64(states->z_x1_ord2.L, coeffs->k1, acc);
    acc = msub64(states->z_y1_ord2.L, coeffs->k1, acc);
    acc = add64(lsh64(states->z_x2_ord2.L, 31), acc);

    acc = lsh64(acc, 1);

    states->z_x2_ord2.L = states->z_x1_ord2.L;
    states->z_x1_ord2.L = states->x0_ord2.L;

    states->z_y2_ord2.L = states->z_y1_ord2.L;
    states->z_y1_ord2.L = (my_sint32)(acc >> 32);

    acc = lsh64(acc, 1);
    states->y0_ord2.L = (my_sint32)(acc >> 32);

    ////////////////////////////////////////////////////////

    states->x0_ord2.R = rsh32(in->R, 1);

    acc = 0;
    acc = mac64(states->x0_ord2.R, coeffs->k2, acc);
    acc = msub64(states->z_y2_ord2.R, coeffs->k2, acc);
    acc = mac64(states->z_x1_ord2.R, coeffs->k1, acc);
    acc = msub64(states->z_y1_ord2.R, coeffs->k1, acc);
    acc = add64(lsh64(states->z_x2_ord2.R, 31), acc);

    acc = lsh64(acc, 1);

    states->z_x2_ord2.R = states->z_x1_ord2.R;
    states->z_x1_ord2.R = states->x0_ord2.R;

    states->z_y2_ord2.R = states->z_y1_ord2.R;
    states->z_y1_ord2.R = (my_sint32)(acc >> 32);

    acc = lsh64(acc, 1);
    states->y0_ord2.R = (my_sint32)(acc >> 32);

    return states->y0_ord2;
}

#endif