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

    float h[TAP_64] = { -0.00037386,  -0.00108526,  -0.00085250,   0.00053048,   0.00206063,   0.00190208,  -0.00080576,  -0.00412507,
-0.00413883,   0.00099226,   0.00748067,   0.00798753,  -0.00071111,  -0.01198145,  -0.01361212,  -0.00047492,
0.01712166,   0.02079374,   0.00290743,  -0.02212723,  -0.02889811,  -0.00668534,   0.02612942,  0.03695297,
0.01157396,  -0.02837492,  -0.04382455,  -0.01700464,   0.02841432,   0.04845285,   0.02217333,  -0.02621595,
0.95163662,  -0.02621595,   0.02217333,   0.04845285,   0.02841432,  -0.01700464,  -0.04382455,  -0.02837492,
0.01157396,   0.03695297,   0.02612942,  -0.00668534,  -0.02889811,  -0.02212723,   0.00290743,   0.02079374,
0.01712166,  -0.00047492,  -0.01361212,  -0.01198145,  -0.00071111,   0.00798753,   0.00748067,   0.00099226,
-0.00413883,  -0.00412507,  -0.00080576,   0.00190208,   0.00206063,   0.00053048,  -0.00085250,  -0.00108526,
-0.00037386 };
    float buffer[TAP_64] = {0};
    //float h[TAP_64] = { 0 };
    riffHeader RIFF;
    riffHeader* ptrRIFF = &RIFF;

    fmtHeader FMT;
    fmtHeader* ptrFMT = &FMT;

    dataHeader DATA;
    dataHeader* ptrDATA = &DATA;

    CircularBuffer circ_Config;

   

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

    //writeHeader(ptrRIFF, ptrFMT, ptrDATA, ptrNewWavFile, &numWrite);
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

void writeHeader(riffHeader* ptrRIFF, fmtHeader* ptrFMT, dataHeader* ptrDATA, FILE* ptrNewWavFile, size_t* numWrite)
{
    numWrite = fwrite(ptrRIFF, sizeof(uint8_t), sizeof(riffHeader), ptrNewWavFile);
    numWrite = fwrite(ptrFMT, sizeof(uint8_t), sizeof(fmtHeader), ptrNewWavFile);
    numWrite = fwrite(ptrDATA, sizeof(uint8_t), sizeof(dataHeader), ptrNewWavFile);
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

    loop = TAP_64/2;

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
        blackman =(float)( 0.42 + 0.5*cosf((M2_PI*i) / (TAP_64 - 1)) + 0.08*cosf((2 * M2_PI*i) / (TAP_64 - 1)));
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

    float* ptrVal = &val;
    
    circ_Config->begin = &buf[0];
    circ_Config->end = &buf[64];

    for (i = (TAP_64 - 1); i >= 0; i--)
    {
        numRead = fread(ptrVal, sizeof(float), 1, readFile);
        buf[i] = *ptrVal;
        seek = fseek(readFile, sizeof(float), SEEK_CUR);
    }

    circ_Config->current = &buf[0];
    circ_Config->last = &buf[64];
}

void Update_CircBuff(float* buf, FILE* readFile, CircularBuffer* circ_Config)
{
    static count = TAP_64;
    int seek;
    float val;
    float* ptrVal = &val;
    size_t numRead;

    circ_Config->current--;
    if (circ_Config->current < circ_Config->begin)
    {
        circ_Config->current = circ_Config->end;
    }

    if (count <= 480000)
    {
        numRead = fread(ptrVal, sizeof(float), 1, readFile);
        *(circ_Config->current) = *ptrVal;
        seek = fseek(readFile, sizeof(float), SEEK_CUR);
    }
    else
    {
        *(circ_Config->current) = 0.0;
    }

}

void FIR_tration(float* h, float* buffer, CircularBuffer* circ_Config, FILE* ptrNewWavFile, FILE* ptrWavFile)
{
    int i;
    int loop;
    size_t read;
    float sum = 0.0;

    int num_written = TAP_64;

    size_t numWrite;

    for (loop = 0; loop < 480000; loop++)
    {
        for (i = 0; i < TAP_64; i++)
        {
            sum += h[i] * *(circ_Config->current);

            if ((++circ_Config->current) > circ_Config->end)
            {
                circ_Config->current = circ_Config->begin;
            }
        }

        numWrite = fwrite(&sum, sizeof(float), 1, ptrNewWavFile);
        numWrite = fwrite(&sum, sizeof(float), 1, ptrNewWavFile);

        Update_CircBuff(buffer, ptrWavFile, circ_Config);
    }
    
    

}
