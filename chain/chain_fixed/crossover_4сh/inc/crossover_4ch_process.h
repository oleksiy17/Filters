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

    xh = mac_f(delay->L, coeffs->negk0, in->L);        // replace by nmac_f
    out.L = mac_f(xh, coeffs->k0, delay->L);
    delay->L = xh;

    xh = mac_f(delay->R, coeffs->negk0, in->R);        // replace by nmac_f
    out.R = mac_f(xh, coeffs->k0, delay->R);
    delay->R = xh;

    return out;
}

static __forceinline tStereo_cross order_2(tStereo_cross *in, coef_2_ord *coeffs, tStereo_cross *delay1, tStereo_cross *delay2)
{
    tStereo_cross out;

    out.L = mac_f(in->L, coeffs->k2, delay1->L);
    delay1->L = mac_f(in->L, coeffs->k1, delay2->L);
    delay1->L = mac_f(out.L, coeffs->negk1, delay1->L);
    delay2->L = mac_f(out.L, coeffs->negk2, in->L);

    out.R = mac_f(in->R, coeffs->k2, delay1->R);
    delay1->R = mac_f(in->R, coeffs->k1, delay2->R);
    delay1->R = mac_f(out.R, coeffs->negk1, delay1->R);
    delay2->R = mac_f(out.R, coeffs->negk2, in->R);

    return out;
}
#endif