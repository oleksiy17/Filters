#include "fir_fixed.h"

void FIR_tration_fxd(my_sint32* h, my_sint32* buffer, CircularBuffer* circ_Config, FILE* ptrNewWavFile, FILE* ptrWavFile, int sampleNum)
{
    int outerIdX;
    int innerIdx;
    my_sint32 sum;
    size_t numWrite;

    for (outerIdX = 0; outerIdX < sampleNum; outerIdX++)               // loop = 1 becouse of init_circular buffer function init 1st valuse ftom 480000
    {
        sum = 0;
        for (innerIdx = 0; innerIdx < TAP_65; innerIdx++)
        {
            sum = mac32(h[innerIdx], *((my_sint32*)(circ_Config->current)), sum);
            Convolution_CircBuff_fxd(circ_Config);
        }

        numWrite = fwrite(&sum, sizeof(my_sint32), 1, ptrNewWavFile);
        numWrite = fwrite(&sum, sizeof(my_sint32), 1, ptrNewWavFile);
        Update_CircBuff_fxd(buffer, ptrWavFile, circ_Config);
    }

}