#ifndef __EFFECT_PROCESS_H__
#define __EFFECT_PROCESS_H__

#include <stdint.h>
#include <stddef.h>

#include "equalizer_process.h"
#include "equalizer_control.h"
#include "fixedpoint.h"


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
    equalizer_states *reset_states;
    size_t i;

    reset_states = (equalizer_states*)states;
    
    for ( i = 0; i < (size_t)BANDS; i++)
    {
        reset_states->x0[i].L = 0;
        reset_states->x1[i].L = 0;
        reset_states->x2[i].L = 0;
        reset_states->y0[i].L = 0;
        reset_states->y1[i].L = 0;
        reset_states->y2[i].L = 0;
        reset_states->w1[i].L = 0;

        reset_states->x0[i].R = 0;
        reset_states->x1[i].R = 0;
        reset_states->x2[i].R = 0;
        reset_states->y0[i].R = 0;
        reset_states->y1[i].R = 0;
        reset_states->y2[i].R = 0;
        reset_states->w2[i].R = 0;
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
    tStereo_eq *a = (tStereo_eq*)audio;
    tStereo_eq wn;

    for (size_t i = 0; i < samples_count; i++)
    {
        for (size_t j = 0; j < 10; j++)
        {
            if (c->a0[j] != 0)
            {

                s->x0[j].L = a[i].L;
                s->x0[j].R = a[i].R;

                wn.L = (c->a1[j] * s->w1[j].L) + (c->a2[j] * s->w2[j].L) + a[i].L;
                s->y0[j].L = (c->b0[j] * wn.L) + (c->b1[j] * s->w1[j].L) + (c->b2[j] * s->w2[j].L);
                a[i].L = s->y0[j].L;

                s->w2[j].L = s->w1[j].L;
                s->w1[j].L = wn.L;


                wn.R = (c->a1[j] * s->w1[j].R) + (c->a2[j] * s->w2[j].R) + a[i].R;
                s->y0[j].R = (c->b0[j] * wn.R) + (c->b1[j] * s->w1[j].R) + (c->b2[j] * s->w2[j].R);
                a[i].R = s->y0[j].R;

                s->w2[j].R = s->w1[j].R;
                s->w1[j].R = wn.R;


                /*wn.R = nmsub_f(c->a1[j], s->w1[j].R, a[i].R);
                wn.R = nmsub_f(c->a2[j], s->w2[j].R, wn.R);

                s->y0[j].R = mul_f(c->b0[j], wn.R);
                s->y0[j].R = mac_f(c->b1[j], s->w1[j].R, s->y0[j].R);
                s->y0[j].R = mac_f(c->b2[j], s->w2[j].R, s->y0[j].R);

                a[i].R = s->y0[j].R;
                
                s->w2[j].L = s->w1[j].L;
                s->w1[j].L = wn.L;

                s->w2[j].R = s->w1[j].R;
                s->w1[j].R = wn.R;*/


            }
        }


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
/*int32_t equalizer_process(
    void const* coeffs,
    void*       states,
    void*       audio,
    size_t      samples_count)
{
    equalizer_coeffs *c = (equalizer_coeffs*)coeffs;
    equalizer_states *s = (equalizer_states*)states;
    tStereo_eq *a = (tStereo_eq*)audio;
    my_float acc;

    for (size_t i = 0; i < samples_count; i++)
    {
        s->x0[0].L = a[i].L;
        s->x0[0].R = a[i].R;


        /// CH_0
        s->y0[0].L = s->x0[0].L;
        s->y0[0].R = s->x0[0].R;

        if (c->a0[0] != 0)
        {
            s->y0[0].L = 0.0;

            s->y0[0].L = mac_f(s->x0[0].L, 0.996292f, s->y0[0].L);
            s->y0[0].L = mac_f(s->x1[0].L, -1.96368f, s->y0[0].L);
            s->y0[0].L = mac_f(s->x2[0].L, 0.967653f, s->y0[0].L);
            s->y0[0].L = nmsub_f(s->y1[0].L, -1.96368f, s->y0[0].L);
            s->y0[0].L = nmsub_f(s->y2[0].L, 0.963945f, s->y0[0].L);

            s->x2[0].L = s->x1[0].L;
            s->x1[0].L = s->x0[0].L;
            s->y2[0].L = s->y1[0].L;
            s->y1[0].L = s->y0[0].L;

            s->y0[0].R = 0.0;

            s->y0[0].R = mac_f(s->x0[0].R, 0.996292f, s->y0[0].R);
            s->y0[0].R = mac_f(s->x1[0].R, -1.96368f, s->y0[0].R);
            s->y0[0].R = mac_f(s->x2[0].R, 0.967653f, s->y0[0].R);
            s->y0[0].R = nmsub_f(s->y1[0].R, -1.96368f, s->y0[0].R);
            s->y0[0].R = nmsub_f(s->y2[0].R, 0.963945f, s->y0[0].R);

            s->x2[0].R = s->x1[0].R;
            s->x1[0].R = s->x0[0].R;
            s->y2[0].R = s->y1[0].R;
            s->y1[0].R = s->y0[0].R;
        }
        
        s->x0[1].L = s->y0[0].L;
        s->x0[1].R = s->y0[0].R;

        ///---------///


         /// CH_1
        s->y0[1].L = s->x0[1].L;
        s->y0[1].R = s->x0[1].R;

        if (c->a0[1] != 0)
        {
            s->y0[1].L = 0.0;

            s->y0[1].L = mac_f(s->x0[1].L, 0.992717f, s->y0[1].L);
            s->y0[1].L = mac_f(s->x1[1].L, -1.92814f, s->y0[1].L);
            s->y0[1].L = mac_f(s->x2[1].L, 0.93646f, s->y0[1].L);
            s->y0[1].L = nmsub_f(s->y1[1].L, -1.92814f, s->y0[1].L);
            s->y0[1].L = nmsub_f(s->y2[1].L, 0.929177f, s->y0[1].L);

            s->x2[1].L = s->x1[1].L;
            s->x1[1].L = s->x0[1].L;
            s->y2[1].L = s->y1[1].L;
            s->y1[1].L = s->y0[1].L;

            s->y0[1].R = 0.0;

            s->y0[1].R = mac_f(s->x0[1].R, 0.992717f, s->y0[1].R);
            s->y0[1].R = mac_f(s->x1[1].R, -1.92814f, s->y0[1].R);
            s->y0[1].R = mac_f(s->x2[1].R, 0.93646f, s->y0[1].R);
            s->y0[1].R = nmsub_f(s->y1[1].R, -1.92814f, s->y0[1].R);
            s->y0[1].R = nmsub_f(s->y2[1].R, 0.929177f, s->y0[1].R);

            s->x2[1].R = s->x1[1].R;
            s->x1[1].R = s->x0[1].R;
            s->y2[1].R = s->y1[1].R;
            s->y1[1].R = s->y0[1].R;
        }


        s->x0[2].L = s->y0[1].L;
        s->x0[2].R = s->y0[1].R;

        ///---------///

        
         /// CH_2
        s->y0[2].L = s->x0[2].L;
        s->y0[2].R = s->x0[2].R;

        if (c->a0[2] != 0)
        {
            s->y0[2].L = 0.0;

            s->y0[2].L = mac_f(s->x0[2].L, 0.937607f, s->y0[2].L);
            s->y0[2].L = mac_f(s->x1[2].L, -1.87521f, s->y0[2].L);
            s->y0[2].L = mac_f(s->x2[2].L, 0.937607f, s->y0[2].L);
            s->y0[2].L = nmsub_f(s->y1[2].L, -1.8732f, s->y0[2].L);
            s->y0[2].L = nmsub_f(s->y2[2].L, 0.877224f, s->y0[2].L);

            s->x2[2].L = s->x1[2].L;
            s->x1[2].L = s->x0[2].L;
            s->y2[2].L = s->y1[2].L;
            s->y1[2].L = s->y0[2].L;

            s->y0[2].R = 0.0;

            s->y0[2].R = mac_f(s->x0[2].R, 0.937607f, s->y0[2].R);
            s->y0[2].R = mac_f(s->x1[2].R, -1.87521f, s->y0[2].R);
            s->y0[2].R = mac_f(s->x2[2].R, 0.937607f, s->y0[2].R);
            s->y0[2].R = nmsub_f(s->y1[2].R, -1.8732f, s->y0[2].R);
            s->y0[2].R = nmsub_f(s->y2[2].R, 0.877224f, s->y0[2].R);

            s->x2[2].R = s->x1[2].R;
            s->x1[2].R = s->x0[2].R;
            s->y2[2].R = s->y1[2].R;
            s->y1[2].R = s->y0[2].R;
        }

        s->x0[3].L = s->y0[2].L;
        s->x0[3].R = s->y0[2].R;

        ///---------///

       
         /// CH_3
        s->y0[3].L = s->x0[3].L;
        s->y0[3].R = s->x0[3].R;

        if (c->a0[3] != 0)
        {
            s->y0[3].L = 0.0;

            s->y0[3].L = mac_f(s->x0[3].L, 0.973727f, s->y0[3].L);
            s->y0[3].L = mac_f(s->x1[3].L, -1.72959f, s->y0[3].L);
            s->y0[3].L = mac_f(s->x2[3].L, 0.770785f, s->y0[3].L);
            s->y0[3].L = nmsub_f(s->y1[3].L, -1.72959f, s->y0[3].L);
            s->y0[3].L = nmsub_f(s->y2[3].L, 0.744511f, s->y0[3].L);

            s->x2[3].L = s->x1[3].L;
            s->x1[3].L = s->x0[3].L;
            s->y2[3].L = s->y1[3].L;
            s->y1[3].L = s->y0[3].L;

            s->y0[3].R = 0.0;

            s->y0[3].R = mac_f(s->x0[3].R, 0.973727f, s->y0[3].R);
            s->y0[3].R = mac_f(s->x1[3].R, -1.72959f, s->y0[3].R);
            s->y0[3].R = mac_f(s->x2[3].R, 0.770785f, s->y0[3].R);
            s->y0[3].R = nmsub_f(s->y1[3].R, -1.72959f, s->y0[3].R);
            s->y0[3].R = nmsub_f(s->y2[3].R, 0.744511f, s->y0[3].R);

            s->x2[3].R = s->x1[3].R;
            s->x1[3].R = s->x0[3].R;
            s->y2[3].R = s->y1[3].R;
            s->y1[3].R = s->y0[3].R;
        }

        s->x0[4].L = s->y0[3].L;
        s->x0[4].R = s->y0[3].R;

        ///---------///


         /// CH_4
        s->y0[4].L = s->x0[4].L;
        s->y0[4].R = s->x0[4].R;

        if (c->a0[4] != 0)
        {
            s->y0[4].L = 0.0;

            s->y0[4].L = mac_f(s->x0[4].L, 0.953714f, s->y0[4].L);
            s->y0[4].L = mac_f(s->x1[4].L, -1.4971f, s->y0[4].L);
            s->y0[4].L = mac_f(s->x2[4].L, 0.596193f, s->y0[4].L);
            s->y0[4].L = nmsub_f(s->y1[4].L, -1.4971f, s->y0[4].L);
            s->y0[4].L = nmsub_f(s->y2[4].L, 0.549907f, s->y0[4].L);

            s->x2[4].L = s->x1[4].L;
            s->x1[4].L = s->x0[4].L;
            s->y2[4].L = s->y1[4].L;
            s->y1[4].L = s->y0[4].L;

            s->y0[4].R = 0.0;

            s->y0[4].R = mac_f(s->x0[4].R, 0.953714f, s->y0[4].R);
            s->y0[4].R = mac_f(s->x1[4].R, -1.4971f, s->y0[4].R);
            s->y0[4].R = mac_f(s->x2[4].R, 0.596193f, s->y0[4].R);
            s->y0[4].R = nmsub_f(s->y1[4].R, -1.4971f, s->y0[4].R);
            s->y0[4].R = nmsub_f(s->y2[4].R, 0.549907f, s->y0[4].R);

            s->x2[4].R = s->x1[4].R;
            s->x1[4].R = s->x0[4].R;
            s->y2[4].R = s->y1[4].R;
            s->y1[4].R = s->y0[4].R;
        }

        s->x0[5].L = s->y0[4].L;
        s->x0[5].R = s->y0[4].R;

        ///---------///

        
         /// CH_5
        s->y0[5].L = s->x0[5].L;
        s->y0[5].R = s->x0[5].R;

        if (c->a0[5] != 0)
        {
            s->y0[5].L = 0.0;

            s->y0[5].L = mac_f(s->x0[5].L, 0.926084f, s->y0[5].L);
            s->y0[5].L = mac_f(s->x1[5].L, -1.10957f, s->y0[5].L);
            s->y0[5].L = mac_f(s->x2[5].L, 0.355139f, s->y0[5].L);
            s->y0[5].L = nmsub_f(s->y1[5].L, -1.10957f, s->y0[5].L);
            s->y0[5].L = nmsub_f(s->y2[5].L, 0.281222f, s->y0[5].L);

            s->x2[5].L = s->x1[5].L;
            s->x1[5].L = s->x0[5].L;
            s->y2[5].L = s->y1[5].L;
            s->y1[5].L = s->y0[5].L;

            s->y0[5].R = 0.0;

            s->y0[5].R = mac_f(s->x0[5].R, 0.926084f, s->y0[5].R);
            s->y0[5].R = mac_f(s->x1[5].R, -1.10957f, s->y0[5].R);
            s->y0[5].R = mac_f(s->x2[5].R, 0.355139f, s->y0[5].R);
            s->y0[5].R = nmsub_f(s->y1[5].R, -1.10957f, s->y0[5].R);
            s->y0[5].R = nmsub_f(s->y2[5].R, 0.281222f, s->y0[5].R);

            s->x2[5].R = s->x1[5].R;
            s->x1[5].R = s->x0[5].R;
            s->y2[5].R = s->y1[5].R;
            s->y1[5].R = s->y0[5].R;
        }

        s->x0[6].L = s->y0[5].L;
        s->x0[6].R = s->y0[5].R;

        ///---------///


         /// CH_6
        s->y0[6].L = s->x0[6].L;
        s->y0[6].R = s->x0[6].R;

        if (c->a0[6] != 0)
        {
            s->y0[6].L = 0.0;

            s->y0[6].L = mac_f(s->x0[6].L, 0.133975f, s->y0[6].L);
            s->y0[6].L = mac_f(s->x1[6].L, 0.267949f, s->y0[6].L);
            s->y0[6].L = mac_f(s->x2[6].L, 0.133975f, s->y0[6].L);
            s->y0[6].L = nmsub_f(s->y1[6].L, -0.535898f, s->y0[6].L);
            s->y0[6].L = nmsub_f(s->y2[6].L, 0.0717968f, s->y0[6].L);

            s->x2[6].L = s->x1[6].L;
            s->x1[6].L = s->x0[6].L;
            s->y2[6].L = s->y1[6].L;
            s->y1[6].L = s->y0[6].L;

            s->y0[6].R = 0.0;

            s->y0[6].R = mac_f(s->x0[6].R, 0.133975f, s->y0[6].R);
            s->y0[6].R = mac_f(s->x1[6].R, 0.267949f, s->y0[6].R);
            s->y0[6].R = mac_f(s->x2[6].R, 0.133975f, s->y0[6].R);
            s->y0[6].R = nmsub_f(s->y1[6].R, -0.535898f, s->y0[6].R);
            s->y0[6].R = nmsub_f(s->y2[6].R, 0.0717968f, s->y0[6].R);

            s->x2[6].R = s->x1[6].R;
            s->x1[6].R = s->x0[6].R;
            s->y2[6].R = s->y1[6].R;
            s->y1[6].R = s->y0[6].R;
        }

        s->x0[7].L = s->y0[6].L;
        s->x0[7].R = s->y0[6].R;

        ///---------///


         /// CH_7
        s->y0[7].L = s->x0[7].L;
        s->y0[7].R = s->x0[7].R;

        if (c->a0[7] != 0)
        {
            s->y0[7].L = 0.0;

            s->y0[7].L = mac_f(s->x0[7].L, 0.891251f, s->y0[7].L);
            s->y0[7].L = mac_f(s->x1[7].L, -5.77114e-017f, s->y0[7].L);
            s->y0[7].L = mac_f(s->x2[7].L, 0.0512479f, s->y0[7].L);
            s->y0[7].L = nmsub_f(s->y1[7].L, -5.77114e-017f, s->y0[7].L);
            s->y0[7].L = nmsub_f(s->y2[7].L, -0.0575011f, s->y0[7].L);

            s->x2[7].L = s->x1[7].L;
            s->x1[7].L = s->x0[7].L;
            s->y2[7].L = s->y1[7].L;
            s->y1[7].L = s->y0[7].L;

            s->y0[7].R = 0.0;

            s->y0[7].R = mac_f(s->x0[7].R, 0.891251f, s->y0[7].R);
            s->y0[7].R = mac_f(s->x1[7].R, -5.77114e-017f, s->y0[7].R);
            s->y0[7].R = mac_f(s->x2[7].R, 0.0512479f, s->y0[7].R);
            s->y0[7].R = nmsub_f(s->y1[7].R, -5.77114e-017f, s->y0[7].R);
            s->y0[7].R = nmsub_f(s->y2[7].R, -0.0575011f, s->y0[7].R);

            s->x2[7].R = s->x1[7].R;
            s->x1[7].R = s->x0[7].R;
            s->y2[7].R = s->y1[7].R;
            s->y1[7].R = s->y0[7].R;
        }

        s->x0[8].L = s->y0[7].L;
        s->x0[8].R = s->y0[7].R;

        ///---------///


         /// CH_8
        s->y0[8].L = s->x0[8].L;
        s->y0[8].R = s->x0[8].R;

        if (c->a0[8] != 0)
        {
            s->y0[8].L = 0.0;

            s->y0[8].L = mac_f(s->x0[8].L, 0.893029f, s->y0[8].L);
            s->y0[8].L = mac_f(s->x1[8].L, 0.248412f, s->y0[8].L);
            s->y0[8].L = mac_f(s->x2[8].L, 0.066762f, s->y0[8].L);
            s->y0[8].L = nmsub_f(s->y1[8].L, 0.248412f, s->y0[8].L);
            s->y0[8].L = nmsub_f(s->y2[8].L, -0.0402088f, s->y0[8].L);

            s->x2[8].L = s->x1[8].L;
            s->x1[8].L = s->x0[8].L;
            s->y2[8].L = s->y1[8].L;
            s->y1[8].L = s->y0[8].L;

            s->y0[8].R = 0.0;

            s->y0[8].R = mac_f(s->x0[8].R, 0.893029f, s->y0[8].R);
            s->y0[8].R = mac_f(s->x1[8].R, 0.248412f, s->y0[8].R);
            s->y0[8].R = mac_f(s->x2[8].R, 0.066762f, s->y0[8].R);
            s->y0[8].R = nmsub_f(s->y1[8].R, 0.248412f, s->y0[8].R);
            s->y0[8].R = nmsub_f(s->y2[8].R, -0.0402088f, s->y0[8].R);

            s->x2[8].R = s->x1[8].R;
            s->x1[8].R = s->x0[8].R;
            s->y2[8].R = s->y1[8].R;
            s->y1[8].R = s->y0[8].R;
        }

        s->x0[9].L = s->y0[8].L;
        s->x0[9].R = s->y0[8].R;

        ///---------///


         /// CH_9
        s->y0[9].L = s->x0[9].L;
        s->y0[9].R = s->x0[9].R;

        if (c->a0[9] != 0)
        {
            s->y0[9].L = 0.0;

            s->y0[9].L = mac_f(s->x0[9].L, 0.909012f, s->y0[9].L);
            s->y0[9].L = mac_f(s->x1[9].L, 0.788571f, s->y0[9].L);
            s->y0[9].L = mac_f(s->x2[9].L, 0.206197f, s->y0[9].L);
            s->y0[9].L = nmsub_f(s->y1[9].L, 0.788571f, s->y0[9].L);
            s->y0[9].L = nmsub_f(s->y2[9].L, 0.115208f, s->y0[9].L);

            s->x2[9].L = s->x1[9].L;
            s->x1[9].L = s->x0[9].L;
            s->y2[9].L = s->y1[9].L;
            s->y1[9].L = s->y0[9].L;

            s->y0[9].R = 0.0;

            s->y0[9].R = mac_f(s->x0[9].R, 0.909012f, s->y0[9].R);
            s->y0[9].R = mac_f(s->x1[9].R, 0.788571f, s->y0[9].R);
            s->y0[9].R = mac_f(s->x2[9].R, 0.206197f, s->y0[9].R);
            s->y0[9].R = nmsub_f(s->y1[9].R, 0.788571f, s->y0[9].R);
            s->y0[9].R = nmsub_f(s->y2[9].R, 0.115208f, s->y0[9].R);

            s->x2[9].R = s->x1[9].R;
            s->x1[9].R = s->x0[9].R;
            s->y2[9].R = s->y1[9].R;
            s->y1[9].R = s->y0[9].R;
        }

        a[i].L = s->y0[9].L;
        a[i].R = s->y0[9].R;
        ///---------///

    }
    return 0;
}*/



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
/*int32_t equalizer_process(
    void const* coeffs,
    void*       states,
    void*       audio,
    size_t      samples_count)
{
    equalizer_coeffs *c = (equalizer_coeffs*)coeffs;
    equalizer_states *s = (equalizer_states*)states;
    tStereo_eq *a = (tStereo_eq*)audio;
    my_float acc;
    for (size_t i = 0; i < samples_count; i++)
    {
        for (size_t j = 0; j < 10; j++)
        {
            if (c->a0[j] != 0)
            {

               /*s->x0[j].L = a[i].L;
                s->x0[j].R = a[i].R;


                s->y0[j].L = mac_f(c->b0[j], s->x0[j].L, s->x1[j].L); 

                s->x1[j].L = mac_f(c->b1[j], s->x0[j].L, s->x2[j].L);
                s->x1[j].L = nmsub_f(c->a1[j], s->y0[j].L, s->x1[j].L);

                s->x2[j].L = mul_f(c->b2[j], s->x0[j].L);
                s->x2[j].L = nmsub_f(c->a2[j], s->y0[j].L, s->x2[j].L);

                a[i].L = s->y0[j].L;


                s->y0[j].R = mac_f(c->b0[j], s->x0[j].R, s->x1[j].R);   

                s->x1[j].R = mac_f(c->b1[j], s->x0[j].R, s->x2[j].R);
                s->x1[j].R = nmsub_f(c->a1[j], s->y0[j].R, s->x1[j].R);

                s->x2[j].R = mul_f(c->b2[j], s->x0[j].R);
                s->x2[j].R = nmsub_f(c->a2[j], s->y0[j].R, s->x2[j].R);

                a[i].R = s->y0[j].R;*/

                /*acc = 0.0;
                s->x0[j].L = a[i].L;
                acc = mac_f(s->x0[j].L, 0.0135342f, acc);
                acc = mac_f(s->x1[j].L, 0.0270684f, acc);
                acc = mac_f(s->x2[j].L, 0.0135342f, acc);
                acc = nmsub_f(s->y1[j].L, -1.53465f, acc);
                acc = nmsub_f(s->y2[j].L, 0.588791f, acc);
               
                s->x2[j].L = s->x1[j].L;
                s->x1[j].L = s->x0[j].L;
                s->y2[j].L = s->y1[j].L;
                s->y1[j].L = acc;

                a[i].L = acc;


                acc = 0.0;
                s->x0[j].R = a[i].R;
                acc = mac_f(s->x0[j].R, 0.0135342f, acc);
                acc = mac_f(s->x1[j].R, 0.0270684f, acc);
                acc = mac_f(s->x2[j].R, 0.0135342f, acc);
                acc = nmsub_f(s->y1[j].R, -1.53465f, acc);
                acc = nmsub_f(s->y2[j].R, 0.588791f, acc);

                s->x2[j].R = s->x1[j].R;
                s->x1[j].R = s->x0[j].R;
                s->y2[j].R = s->y1[j].R;
                s->y1[j].R = acc;

                a[i].R = acc;*/

               /*acc = 0.0;
               s->x0[j].L = a[i].L;
               acc = mac_f(s->x0[j].L, c->b0[j], acc);
               acc = mac_f(s->x1[j].L, c->b1[j], acc);
               acc = mac_f(s->x2[j].L, c->b2[j], acc);
               acc = nmsub_f(s->y1[j].L, c->a1[j], acc);
               acc = nmsub_f(s->y2[j].L, c->a2[j], acc);

               s->x2[j].L = s->x1[j].L;
               s->x1[j].L = s->x0[j].L;
               s->y2[j].L = s->y1[j].L;
               s->y1[j].L = acc;

               a[i].L = acc;


               acc = 0.0;
               s->x0[j].R = a[i].R;
               acc = mac_f(s->x0[j].R, c->b0[j], acc);
               acc = mac_f(s->x1[j].R, c->b1[j], acc);
               acc = mac_f(s->x2[j].R, c->b2[j], acc);
               acc = nmsub_f(s->y1[j].R, c->a1[j], acc);
               acc = nmsub_f(s->y2[j].R, c->a2[j], acc);

               s->x2[j].R = s->x1[j].R;
               s->x1[j].R = s->x0[j].R;
               s->y2[j].R = s->y1[j].R;
               s->y1[j].R = acc;

               a[i].R = acc;*
                
            }
        }

        
    }
    return 0;
}
*/

#endif
