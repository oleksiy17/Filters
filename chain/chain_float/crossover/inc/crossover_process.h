#ifndef __CROSSOVER_PROCESS_H__
#define __CROSSOVER_PROCESS_H__

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


//extern tStereo_cross order_1(tStereo_cross *in, coef_1_ord *coeffs, tStereo_cross *delay);
//extern tStereo_cross order_2(tStereo_cross *in, coef_2_ord *coeffs, tStereo_cross *delay1, tStereo_cross *delay2);


static __forceinline tStereo_cross order_1(tStereo_cross *in, coef_1_ord *coeffs, tStereo_cross *delay)
{
    float xh;
    tStereo_cross out;

    xh = nmsub_f(delay->L, coeffs->k0, in->L);        // replace by nmac_f
    out.L = mac_f(xh, coeffs->k0, delay->L);
    delay->L = xh; 

    xh = nmsub_f(delay->R, coeffs->k0, in->R);        // replace by nmac_f
    out.R = mac_f(xh, coeffs->k0, delay->R);
    delay->R = xh;

    return out;
}


static __forceinline tStereo_cross order_2(tStereo_cross *in, coef_2_ord *c, states_2* s)
{
    tStereo_cross wn;
    tStereo_cross out;

    wn.L = (c->a1 * s->w1.L) + (c->a2 * s->w2.L) + in->L;
    out.L = (c->b0 * wn.L) + (c->b1 * s->w1.L) + (c->b2 * s->w2.L);
   
    s->w2.L = s->w1.L;
    s->w1.L = wn.L;

    wn.R = (c->a1 * s->w1.R) + (c->a2 * s->w2.R) + in->R;
    out.R = (c->b0 * wn.R) + (c->b1 * s->w1.R) + (c->b2 * s->w2.R);

    s->w2.R = s->w1.R;
    s->w1.R = wn.R;

    return out;
}

/*
static __forceinline tStereo_cross order_2(tStereo_cross *in, coef_2_ord *coeffs, states_2 *states)
{
    tStereo_cross out;

    out.L = 0.0;
    out.L = nmsub_f(in->L, coeffs->k2, out.L);
    out.L = mac_f(states->x_1.L, coeffs->k1, out.L);
    out.L = nmsub_f(states->y_1.L, coeffs->k1, out.L);
    out.L = mac_f(states->y_2.L, coeffs->k2, out.L);
    out.L = add_f(states->x_2.L, out.L);

    states->x_2.L = states->x_1.L;
    states->x_1.L = in->L;
    states->y_2.L = states->y_1.L;
    states->y_1.L = out.L;

    out.R = 0.0;
    out.R = nmsub_f(in->R, coeffs->k2, out.R);
    out.R = mac_f(states->x_1.R, coeffs->k1, out.R);
    out.R = nmsub_f(states->y_1.R, coeffs->k1, out.R);
    out.R = mac_f(states->y_2.R, coeffs->k2, out.R);
    out.R = add_f(states->x_2.R, out.R);

    states->x_2.R = states->x_1.R;
    states->x_1.R = in->R;
    states->y_2.R = states->y_1.R;
    states->y_1.R = out.R;
   

    return out;
}*/

#endif