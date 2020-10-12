#include "Filter_process.h"
#include "Filter_control.h"
#include "main.h"


int main()
{
    FILE wavFile;
    FILE* ptrWavFile;

    FILE newWavFile;
    FILE* ptrNewWavFile = &newWavFile;

    char* fileAdd;
    char* newFileAdd;

    size_t numRead = 0;
    size_t numWrite = 0;

    /*static float h[TAP_64] = { -0.00037386,  -0.00108526,  -0.00085250,   0.00053048,   0.00206063,   0.00190208,  -0.00080576,  -0.00412507,
-0.00413883,   0.00099226,   0.00748067,   0.00798753,  -0.00071111,  -0.01198145,  -0.01361212,  -0.00047492,
0.01712166,   0.02079374,   0.00290743,  -0.02212723,  -0.02889811,  -0.00668534,   0.02612942,  0.03695297,
0.01157396,  -0.02837492,  -0.04382455,  -0.01700464,   0.02841432,   0.04845285,   0.02217333,  -0.02621595,
0.95163662,  -0.02621595,   0.02217333,   0.04845285,   0.02841432,  -0.01700464,  -0.04382455,  -0.02837492,
0.01157396,   0.03695297,   0.02612942,  -0.00668534,  -0.02889811,  -0.02212723,   0.00290743,   0.02079374,
0.01712166,  -0.00047492,  -0.01361212,  -0.01198145,  -0.00071111,   0.00798753,   0.00748067,   0.00099226,
-0.00413883,  -0.00412507,  -0.00080576,   0.00190208,   0.00206063,   0.00053048,  -0.00085250,  -0.00108526,
-0.00037386 };*/
    static float h[TAP_64] = { -0.00112908507,   0.00154856213,  -0.00017314556,  -0.00183074641,   0.00212721904,   0.00020970832,
-0.00287886830,   0.00251786191,   0.00079872872,  -0.00303662719,  0.00162731680,   0.00060903567,
-0.00000029979,  -0.00107897700,  -0.00266443131,   0.00836662042,  -0.00449003157,  -0.01182883190,
0.02232157836,  -0.00575360036,  -0.02842951281,   0.03919585291,  -0.00155409794,  -0.05104705834,
0.05392439940,   0.00981564039,  -0.07487481025,   0.06114398560,   0.02685304194,  -0.09316994126,
0.05781345146,   0.04486140921,   0.90041130551,   0.04486140921,   0.05781345146,  -0.09316994126,
0.02685304194,   0.06114398560,  -0.07487481025,   0.00981564039,   0.05392439940,  -0.05104705834,
-0.00155409794,   0.03919585291,  -0.02842951281,  -0.00575360036,   0.02232157836,  -0.01182883190,
-0.00449003157,   0.00836662042,  -0.00266443131,  -0.00107897700,  -0.00000029979,   0.00060903567,
0.00162731680,  -0.00303662719,   0.00079872872,  0.00251786191,  -0.00287886830,   0.00020970832,
0.00212721904,  -0.00183074641,  -0.00017314556,   0.00154856213,  -0.00112908507 };


    static float buffer[TAP_64] = { 0 };
    //float h[TAP_64] = { 0 };
    riffHeader RIFF;
    riffHeader* ptrRIFF = &RIFF;

    fmtHeader FMT;
    fmtHeader* ptrFMT = &FMT;

    dataHeader DATA;
    dataHeader* ptrDATA = &DATA;

    static CircularBuffer circ_Config;
    float tot = 0;
    for (int j = 0; j < TAP_64; j++)
    {
        tot += h[j];
    }

    printf("tot: %f\n", tot);

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


void Init_CircBuff(float* buf, FILE* readFile, CircularBuffer* circ_Config, size_t* numRead)
{
    int i;
    int seek;
    float val;
    float* ptrVal;

    val = 0.0;
    ptrVal = &val;

    numRead = fread((float*)ptrVal, (size_t)sizeof(float), (size_t)1, (FILE*)readFile);
    seek = fseek(readFile, sizeof(float), SEEK_CUR);
    buf[0] = val;

    circ_Config->begin = &buf[0];
    circ_Config->end = &buf[64];
    circ_Config->current = &buf[0];

    for (i = 1; i < TAP_64; i++)
    {
        buf[i] = 0.0;
    }
}

void SetCurrent_CircBuff(CircularBuffer* circ_Config)
{
    if (circ_Config->current == circ_Config->begin)
    {
        circ_Config->current = circ_Config->end;
    }
    else
    {
        circ_Config->current--;
    }
}

void Convolution_CircBuff(CircularBuffer* circ_Config)
{
    if (circ_Config->current == circ_Config->end)
    {
        circ_Config->current = circ_Config->begin;
    }
    else
    {
        circ_Config->current++;
    }
}

void Update_CircBuff(float* buf, FILE* readFile, CircularBuffer* circ_Config)
{
    int seek;
    float val;
    float* ptrVal;
    size_t numRead;

    val = 0.0;
    ptrVal = &val;

    SetCurrent_CircBuff(circ_Config);   //set current posuition for re-writting
    numRead = fread(ptrVal, sizeof(float), 1, readFile);
    seek = fseek(readFile, sizeof(float), SEEK_CUR);

    *(circ_Config->current) = val;
}

void FIR_tration(float* h, float* buffer, CircularBuffer* circ_Config, FILE* ptrNewWavFile, FILE* ptrWavFile)
{
    int outerIdX;
    int innerIdx;
    float sum;
    size_t numWrite;

    

    for (outerIdX = 1; outerIdX < 480000; outerIdX++)               // loop = 1 becouse of init_circular buffer function init 1st valuse ftom 480000
    {
        sum = 0.0;
        for (innerIdx = 0; innerIdx < TAP_64; innerIdx++)
        {
            sum += h[innerIdx] * *(circ_Config->current);
            Convolution_CircBuff(circ_Config);
        }

        numWrite = fwrite(&sum, sizeof(float), 1, ptrNewWavFile);
        numWrite = fwrite(&sum, sizeof(float), 1, ptrNewWavFile);

        Update_CircBuff(buffer, ptrWavFile, circ_Config);
    }
}

