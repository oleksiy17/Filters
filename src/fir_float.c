#include "fir_float.h"


/*static float h[TAP_64] = { -0.00112908507,   0.00154856213,  -0.00017314556,  -0.00183074641,   0.00212721904,   0.00020970832,
                -0.00287886830,   0.00251786191,   0.00079872872,  -0.00303662719,   0.00162731680,   0.00060903567,
                -0.00000029979,  -0.00107897700,  -0.00266443131,   0.00836662042,  -0.00449003157,  -0.01182883190,
                 0.02232157836,  -0.00575360036,  -0.02842951281,   0.03919585291,  -0.00155409794,  -0.05104705834,
                 0.05392439940,   0.00981564039,  -0.07487481025,   0.06114398560,   0.02685304194,  -0.09316994126,
                 0.05781345146,   0.04486140921,   0.90041130551,   0.04486140921,   0.05781345146,  -0.09316994126,
                 0.02685304194,   0.06114398560,  -0.07487481025,   0.00981564039,   0.05392439940,  -0.05104705834,
                -0.00155409794,   0.03919585291,  -0.02842951281,  -0.00575360036,   0.02232157836,  -0.01182883190,
                -0.00449003157,   0.00836662042,  -0.00266443131,  -0.00107897700,  -0.00000029979,   0.00060903567,
                 0.00162731680,  -0.00303662719,   0.00079872872,   0.00251786191,  -0.00287886830,   0.00020970832,
                 0.00212721904,  -0.00183074641,  -0.00017314556,   0.00154856213,  -0.00112908507 };*/
//h[TAP_64] = { 0 };

void FIR_tration(float* h, float* buffer, CircularBuffer* circ_Config, FILE* ptrNewWavFile, FILE* ptrWavFile, int sampleNum)
{
    int outerIdX;
    int innerIdx;
    float sum;
    size_t numWrite;

    for (outerIdX = 0; outerIdX < sampleNum; outerIdX++)               // loop = 1 becouse of init_circular buffer function init 1st valuse ftom 480000
    {
        sum = 0.0;
        for (innerIdx = 0; innerIdx < TAP_64; innerIdx++)
        {
            sum += h[innerIdx] * *((float*)(circ_Config->current));
            Convolution_CircBuff(circ_Config);
        }

        numWrite = fwrite(&sum, sizeof(float), 1, ptrNewWavFile);
        numWrite = fwrite(&sum, sizeof(float), 1, ptrNewWavFile);
        Update_CircBuff(buffer, ptrWavFile, circ_Config);
    }

}

void FIR_coeff_calc(float* h, float sampRate, float cof1, float cof2)
{
    float hd;

    float smooth_low = ((float)sampRate * cof1) - 250.0;
    float smooth_high = ((float)sampRate * cof2) + 250.0;
    float blackman;
    float div;

    int loop;
    int i;

    loop = TAP_64 / 2;

    printf("Sample Rate : %f\nsmooth_low = %f\nsmooth_high = %f\n", sampRate, smooth_low, smooth_high);

    hd = 1.0 - 2.0*(smooth_high - smooth_low);
    blackman = 1.0;
    h[0] = hd * blackman;
    div = h[0];
    h[0] /= div;

    h[64] = h[0];
    printf("h[%d] = h[%d] = %f\n", 0, 64, h[0]);

    for (i = 1; i <= loop; i++)
    {

        hd = (float)(2.0 * (smooth_low * (sinf(M2_PI * smooth_low *i) / (M2_PI * smooth_low *i)) - smooth_high * (sinf(M2_PI * smooth_high *i) / (M2_PI * smooth_high *i))));
        blackman = (float)(0.42 + 0.5*cosf((M2_PI*i) / (TAP_64 - 1)) + 0.08*cosf((2 * M2_PI*i) / (TAP_64 - 1)));
        h[i] = (hd * blackman) / div;
        h[TAP_64 - 1 - i] = h[i];

        printf("h[%d] = h[%d] = %.15f\n", (TAP_64 - 1 - i), i, h[i]);
    }
}

