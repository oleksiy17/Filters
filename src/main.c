#include "Filter_process.h"
#include "Filter_control.h"
#include "main.h"


int main()
{
    FILE* ptrWavFile;
    FILE newWavFile;
    FILE* ptrNewWavFile = &newWavFile;

    char* fileAdd;
    char* newFileAdd;

    size_t numRead = 0;
    size_t numWrite = 0;

    riffHeader RIFF;
    riffHeader* ptrRIFF = &RIFF;
    fmtHeader FMT;
    fmtHeader* ptrFMT = &FMT;
    dataHeader DATA;
    dataHeader* ptrDATA = &DATA;

    static CircularBuffer circ_Config;
    static float buffer[TAP_64] = { 0 };
    static float h[TAP_64] = { -0.00112908507,   0.00154856213,  -0.00017314556,  -0.00183074641,   0.00212721904,   0.00020970832,
                -0.00287886830,   0.00251786191,   0.00079872872,  -0.00303662719,   0.00162731680,   0.00060903567,
                -0.00000029979,  -0.00107897700,  -0.00266443131,   0.00836662042,  -0.00449003157,  -0.01182883190,
                 0.02232157836,  -0.00575360036,  -0.02842951281,   0.03919585291,  -0.00155409794,  -0.05104705834,
                 0.05392439940,   0.00981564039,  -0.07487481025,   0.06114398560,   0.02685304194,  -0.09316994126,
                 0.05781345146,   0.04486140921,   0.90041130551,   0.04486140921,   0.05781345146,  -0.09316994126,
                 0.02685304194,   0.06114398560,  -0.07487481025,   0.00981564039,   0.05392439940,  -0.05104705834,
                -0.00155409794,   0.03919585291,  -0.02842951281,  -0.00575360036,   0.02232157836,  -0.01182883190,
                -0.00449003157,   0.00836662042,  -0.00266443131,  -0.00107897700,  -0.00000029979,   0.00060903567,
                 0.00162731680,  -0.00303662719,   0.00079872872,   0.00251786191,  -0.00287886830,   0.00020970832,
                 0.00212721904,  -0.00183074641,  -0.00017314556,   0.00154856213,  -0.00112908507 };

    

    fileAdd = "C:/Filters/test_signal/test.wav";
    newFileAdd = "C:/Filters/test_signal/fir.wav";

    if ((ptrWavFile = fopen(fileAdd, "rb")) == NULL)      /*  Open existance .wav file    */
    {
        printf("ERROR in opening existance file!\n");
    }

    if ((ptrNewWavFile = fopen(newFileAdd, "w+b")) == NULL)      /*  Open existance .wav file    */
    {
        printf("ERROR in opening new file!\n");
    }

    readHeader(ptrRIFF, ptrFMT, ptrDATA, ptrWavFile, &numRead);

    //FIR_coeff_calc(h, ptrFMT->sample_rate, 0.3, 0.35);
    Init_CircBuff(buffer, ptrWavFile, &circ_Config, &numRead);

    numWrite = fwrite(ptrRIFF, sizeof(uint8_t), sizeof(riffHeader), ptrNewWavFile);
    numWrite = fwrite(ptrFMT, sizeof(uint8_t), sizeof(fmtHeader), ptrNewWavFile);
    numWrite = fwrite(ptrDATA, sizeof(uint8_t), sizeof(dataHeader), ptrNewWavFile);

    FIR_tration(h, buffer, &circ_Config, ptrNewWavFile, ptrWavFile);

    int a = fclose(ptrWavFile);
    a = fclose(ptrNewWavFile);

    return 0;
}

void readHeader(riffHeader* ptrRIFF, fmtHeader* ptrFMT, dataHeader* ptrDATA, FILE* ptrWavFile, size_t* numRead)
{
    numRead = fread(ptrRIFF, sizeof(uint8_t), sizeof(riffHeader), ptrWavFile);
    if (numRead == sizeof(riffHeader))
    {
        printf("ALL RIFF HEADER READED : %d from %d!\n", numRead, sizeof(riffHeader));
    }

    numRead = fread(ptrFMT, sizeof(uint8_t), sizeof(fmtHeader), ptrWavFile);
    if (numRead == sizeof(fmtHeader))
    {
        printf("ALL FMT HEADER READED : %d from %d!\n", numRead, sizeof(fmtHeader));
    }

    numRead = fread(ptrDATA, sizeof(uint8_t), sizeof(dataHeader), ptrWavFile);
    if (numRead == sizeof(dataHeader))
    {
        printf("ALL DATA HEADER READED : %d from %d!\n", numRead, sizeof(dataHeader));
    }

}


