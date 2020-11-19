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
    
    reset_states->in = (tStereo_cross){ 0.0, 0.0 };

    reset_states->cd1_ord1[0] = (tStereo_cross) { 0.0, 0.0 };
    reset_states->cd1_ord1[1] = (tStereo_cross) { 0.0, 0.0 };

    reset_states->cd1_delay_1[0] = (tStereo_cross) { 0.0, 0.0 };
    reset_states->cd1_delay_1[1] = (tStereo_cross) { 0.0, 0.0 };
    reset_states->cd1_delay_1[2] = (tStereo_cross) { 0.0, 0.0 };

    reset_states->cd1_delay_2[0] = (tStereo_cross) { 0.0, 0.0 };
    reset_states->cd1_delay_2[1] = (tStereo_cross) { 0.0, 0.0 };
    reset_states->cd1_delay_2[2] = (tStereo_cross) { 0.0, 0.0 };


    reset_states->norm_ord1[0] = (tStereo_cross) { 0.0, 0.0 };
    reset_states->norm_ord1[1] = (tStereo_cross) { 0.0, 0.0 };

    reset_states->norm_delay_1[0] = (tStereo_cross) { 0.0, 0.0 };
    reset_states->norm_delay_1[1] = (tStereo_cross) { 0.0, 0.0 };
    reset_states->norm_delay_1[2] = (tStereo_cross) { 0.0, 0.0 };

    reset_states->norm_delay_2[0] = (tStereo_cross) { 0.0, 0.0 };
    reset_states->norm_delay_2[1] = (tStereo_cross) { 0.0, 0.0 };
    reset_states->norm_delay_2[2] = (tStereo_cross) { 0.0, 0.0 };


    reset_states->cd2_ord1[0] = (tStereo_cross) { 0.0, 0.0 };
    reset_states->cd2_ord1[1] = (tStereo_cross) { 0.0, 0.0 };

    reset_states->cd2_delay_1[0] = (tStereo_cross) { 0.0, 0.0 };
    reset_states->cd2_delay_1[1] = (tStereo_cross) { 0.0, 0.0 };
    reset_states->cd2_delay_1[2] = (tStereo_cross) { 0.0, 0.0 };

    reset_states->cd2_delay_2[0] = (tStereo_cross) { 0.0, 0.0 };
    reset_states->cd2_delay_2[1] = (tStereo_cross) { 0.0, 0.0 };
    reset_states->cd2_delay_2[2] = (tStereo_cross) { 0.0, 0.0 };


    reset_states->cd3_ord1[0] = (tStereo_cross) { 0.0, 0.0 };
    reset_states->cd3_ord1[1] = (tStereo_cross) { 0.0, 0.0 };

    reset_states->cd3_delay_1[0] = (tStereo_cross) { 0.0, 0.0 };
    reset_states->cd3_delay_1[1] = (tStereo_cross) { 0.0, 0.0 };
    reset_states->cd3_delay_1[2] = (tStereo_cross) { 0.0, 0.0 };

    reset_states->cd3_delay_2[0] = (tStereo_cross) { 0.0, 0.0 };
    reset_states->cd3_delay_2[1] = (tStereo_cross) { 0.0, 0.0 };
    reset_states->cd3_delay_2[2] = (tStereo_cross) { 0.0, 0.0 };

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
    crossover_coeffs* pr_coeffs;
    crossover_states* pr_states;
    audio_buf_cross* pr_audio;

    split_band band;

    tStereo_cross y_ord_1;
    tStereo_cross y_ord_2;
    tStereo_cross sum;
    tStereo_cross acc_ord_2;       // store data result of 1-st order all-pass
    
    tStereo_cross low_pass;
    tStereo_cross high_pass;

    pr_coeffs = (crossover_coeffs*)coeffs;
    pr_states = (crossover_states*)states;
    pr_audio = (audio_buf_cross*)audio;
 

    for (my_uint32 i = 0; i < samples_count; i++)
    {
        pr_states->in = ((tStereo_cross*)pr_audio->audio)[i];    

        // Cascade 1 Parallel 1
        y_ord_1 = order_1(&pr_states->in, &pr_coeffs->band[MID_CUTOFF].ord_1, &pr_states->cd1_ord1[PAR_1]);
        y_ord_2 = order_2(&pr_states->in, &pr_coeffs->band[MID_CUTOFF].ord_2, &pr_states->cd1_delay_1[PAR_1], &pr_states->cd1_delay_2[PAR_1]);
        acc_ord_2 = y_ord_2;
        
        sum.L = add_f(y_ord_1.L, y_ord_2.L);
        sum.L = div_f(sum.L, 2);

        sum.R = add_f(y_ord_1.R, y_ord_2.R);
        sum.R = div_f(sum.R, 2);

        //  Cascade 1 Parallel 2
        y_ord_1 = order_1(&sum, &pr_coeffs->band[MID_CUTOFF].ord_1, &pr_states->cd1_ord1[PAR_2]);
        y_ord_2 = order_2(&sum, &pr_coeffs->band[MID_CUTOFF].ord_2, &pr_states->cd1_delay_1[PAR_2], &pr_states->cd1_delay_2[PAR_2]);

        sum.L = add_f(y_ord_1.L, y_ord_2.L);
        sum.L = div_f(sum.L, 2);

        sum.R = add_f(y_ord_1.R, y_ord_2.R);
        sum.R = div_f(sum.R, 2);

        low_pass = sum;
        
        //  Cascade 1 Serial
        y_ord_1 = order_1(&acc_ord_2, &pr_coeffs->band[MID_CUTOFF].ord_1, &pr_states->cd1_ord1[SERIAL]);

        high_pass.L = sub_f(y_ord_1.L, low_pass.L);
        high_pass.R = sub_f(y_ord_1.R, low_pass.R);      
        
        //////////////////////////////////////////////////////////////////////////////
        //  normalisation low-pass

        y_ord_1 = order_1(&low_pass, &pr_coeffs->band[HIG_CUTOFF].ord_1, &pr_states->norm_ord1[NORM_LOW]);        
        y_ord_2 = order_2(&y_ord_1, &pr_coeffs->band[HIG_CUTOFF].ord_2, &pr_states->norm_delay_1[NORM_LOW], &pr_states->norm_delay_2[NORM_LOW]);        // error
        
        
        // Cascade 2 Parallel 1
        y_ord_1 = order_1(&y_ord_2, &pr_coeffs->band[LOW_CUTOFF].ord_1, &pr_states->cd2_ord1[PAR_1]);
        y_ord_2 = order_2(&y_ord_2, &pr_coeffs->band[LOW_CUTOFF].ord_2, &pr_states->cd2_delay_1[PAR_1], &pr_states->cd2_delay_2[PAR_1]);
        acc_ord_2 = y_ord_2;
       
        sum.L = add_f(y_ord_1.L, y_ord_2.L);
        sum.L = div_f(sum.L, 2);

        sum.R = add_f(y_ord_1.R, y_ord_2.R);
        sum.R = div_f(sum.R, 2);  
      
        //  Cascade 2 Parallel 2
        y_ord_1 = order_1(&sum, &pr_coeffs->band[LOW_CUTOFF].ord_1, &pr_states->cd2_ord1[PAR_2]);  
        y_ord_2 = order_2(&sum, &pr_coeffs->band[LOW_CUTOFF].ord_2, &pr_states->cd2_delay_1[PAR_2], &pr_states->cd2_delay_2[PAR_2]);
        
        sum.L = add_f(y_ord_1.L, y_ord_2.L);
        sum.L = div_f(sum.L, 2);

        sum.R = add_f(y_ord_1.R, y_ord_2.R);
        sum.R = div_f(sum.R, 2);

        ((tStereo_cross*)pr_audio->cross_b.band_1)[i] = sum;                                                ///////////// BAND_1

        //  Cascade 2 Serial
        y_ord_1 = order_1(&acc_ord_2, &pr_coeffs->band[LOW_CUTOFF].ord_1, &pr_states->cd2_ord1[SERIAL]);
        
        ((tStereo_cross*)pr_audio->cross_b.band_2)[i].L = sub_f(y_ord_1.L, sum.L);                          ///////////// BAND_2
        ((tStereo_cross*)pr_audio->cross_b.band_2)[i].R = sub_f(y_ord_1.R, sum.R);


        //////////////////////////////////////////////////////////////////////////////
        //  normalisation high-pass

        y_ord_1 = order_1(&high_pass, &pr_coeffs->band[LOW_CUTOFF].ord_1, &pr_states->norm_ord1[NORM_HIG]);
        y_ord_2 = order_2(&y_ord_1, &pr_coeffs->band[LOW_CUTOFF].ord_2, &pr_states->norm_delay_1[NORM_HIG], &pr_states->norm_delay_2[NORM_HIG]);        // error
        tStereo_cross norm = y_ord_2;
        
        // Cascade 2 Parallel 1
        y_ord_1 = order_1(&y_ord_2, &pr_coeffs->band[HIG_CUTOFF].ord_1, &pr_states->cd3_ord1[PAR_1]);
        y_ord_2 = order_2(&y_ord_2, &pr_coeffs->band[HIG_CUTOFF].ord_2, &pr_states->cd3_delay_1[PAR_1], &pr_states->cd3_delay_2[PAR_1]);
        acc_ord_2 = y_ord_2;
        
        sum.L = add_f(y_ord_1.L, y_ord_2.L);
        sum.L = div_f(sum.L, 2);

        sum.R = add_f(y_ord_1.R, y_ord_2.R);
        sum.R = div_f(sum.R, 2);
        
        //  Cascade 2 Parallel 2
        y_ord_1 = order_1(&sum, &pr_coeffs->band[HIG_CUTOFF].ord_1, &pr_states->cd3_ord1[PAR_2]);
        y_ord_2 = order_2(&sum, &pr_coeffs->band[HIG_CUTOFF].ord_2, &pr_states->cd3_delay_1[PAR_2], &pr_states->cd3_delay_2[PAR_2]);

        sum.L = add_f(y_ord_1.L, y_ord_2.L);
        sum.L = div_f(sum.L, 2);

        sum.R = add_f(y_ord_1.R, y_ord_2.R);
        sum.R = div_f(sum.R, 2);
        
        ((tStereo_cross*)pr_audio->cross_b.band_3)[i] = sum;                                                ///////////// BAND_3
   
        //  Cascade 2 Serial
        y_ord_1 = order_1(&acc_ord_2, &pr_coeffs->band[HIG_CUTOFF].ord_1, &pr_states->cd3_ord1[SERIAL]);

        ((tStereo_cross*)pr_audio->cross_b.band_4)[i].L = sub_f(y_ord_1.L, sum.L);                          ///////////// BAND_4
        ((tStereo_cross*)pr_audio->cross_b.band_4)[i].R = sub_f(y_ord_1.R, sum.R);

        /*((tStereo_cross*)pr_audio->audio)[i].L = ((tStereo_cross*)pr_audio->cross_b.band_1)[i].L;
        ((tStereo_cross*)pr_audio->audio)[i].R = ((tStereo_cross*)pr_audio->cross_b.band_4)[i].L;*/
    }
}

tStereo_cross order_1(tStereo_cross *in, coef_1_ord *coeffs, tStereo_cross *delay )
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

tStereo_cross order_2(tStereo_cross *in, coef_2_ord *coeffs, tStereo_cross *delay1, tStereo_cross *delay2)
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

