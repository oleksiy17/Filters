#ifndef __EFFECT_PROCESS_H__
#define __EFFECT_PROCESS_H__

#include <stdint.h>
#include <stddef.h>

#include "biquad_process.h"
#include "biquad_control.h"

#include "fixedpoint.h"

#define SCALE   1
#define NORM    3

/*******************************************************************************
 * Provides with the required data sizes for states. It is caller responsibility
 *   to allocate enough memory (bytes) for them.
 * 
 * @param[out] states_bytes required data size for storing states
 * 
 * @return 0 if success, non-zero error code otherwise
 ******************************************************************************/
int32_t biquad_process_get_sizes(
    size_t*     states_bytes)
{
    *states_bytes = sizeof(biquad_states);
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
int32_t biquad_reset(
    void const* coeffs,
    void*       states)
{
    biquad_states *reset_states = (biquad_states*)states;

    reset_states->x0.L = 0;
    reset_states->x1.L = 0;
    reset_states->x2.L = 0;
    reset_states->y0.L = 0;
    reset_states->y1.L = 0;
    reset_states->y2.L = 0;

    reset_states->x0.R = 0;
    reset_states->x1.R = 0;
    reset_states->x2.R = 0;
    reset_states->y0.R = 0;
    reset_states->y1.R = 0;
    reset_states->y2.R = 0;

    reset_states->error.L = 0;
    reset_states->error.R = 0;

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
int32_t biquad_process(
    void const* coeffs,
    void*       states,
    void*       audio,
    size_t      samples_count)
{
    biquad_coeffs *c = (biquad_coeffs*)coeffs;
    biquad_states *s = (biquad_states*)states;
    bqStereo *a = (bqStereo*)audio;
    my_sint64 acc = 0;

    for (size_t i = 0; i < samples_count; i++)
    {  
        if (c->a0 != 0)
        {
            s->x0.L = rsh32(a[i].L, SCALE);
            s->x0.R = rsh32(a[i].R, SCALE);

            acc = 0;
            acc = add64(acc, s->error.L);
            acc = mac64(c->b0, s->x0.L, acc);
            acc = mac64(c->b1, s->x1.L, acc);
            acc = mac64(c->b2, s->x2.L, acc);
            acc = msub64(c->a1, s->y1.L, acc);
            acc = msub64(c->a2, s->y2.L, acc);

            s->error.L = (my_sint32)acc;    // lower 32 bits
            acc = lsh64(acc, NORM);
            acc = acc >> 32;                // higher 32 bits
            a[i].L = lsh32((my_sint32)acc, SCALE);

            s->x2.L = s->x1.L;
            s->x1.L = s->x0.L;
            s->y2.L = s->y1.L;
            s->y1.L = (my_sint32)acc;

            acc = 0;
            acc = add64(acc, s->error.R);
            acc = mac64(c->b0, s->x0.R, acc);
            acc = mac64(c->b1, s->x1.R, acc);
            acc = mac64(c->b2, s->x2.R, acc);
            acc = msub64(c->a1, s->y1.R, acc);
            acc = msub64(c->a2, s->y2.R, acc);

            s->error.R = (my_sint32)acc;
            acc = lsh64(acc, NORM);
            acc = acc >> 32;
            a[i].R = lsh32((my_sint32)acc, SCALE);;

            s->x2.R = s->x1.R;
            s->x1.R = s->x0.R;
            s->y2.R = s->y1.R;
            s->y1.R = (my_sint32)acc; 
        }
    }
    return 0;
}

#endif
