#include "main.h"

int main()
{
    static FILE* ptrWavFile;

    FILE newWavFIR;
    FILE* ptrNewWavFIR = &newWavFIR;
    FILE newWavIIR;
    FILE* ptrNewWavIIR = &newWavIIR;


    char* fileAdd;
    char* newFileFIR;
    char* newFileIIR;

    size_t numRead = 0;
    size_t numWrite = 0;

    riffHeader RIFF;
    riffHeader* ptrRIFF = &RIFF;
    fmtHeader FMT;
    fmtHeader* ptrFMT = &FMT;
    dataHeader DATA;
    dataHeader* ptrDATA = &DATA;

    int diviation = 0;
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

    static my_sint32 buffer_fxd[TAP_64] = { 0 };
    static my_sint32 h_fxd[TAP_64] = { -2424691, 3325511, -371827, -3931498, 4568168, 450345,
                                -6182322, 5407067, 1715256, -6521107, 3494636, 1307894,
                                -643, -2317085, -5721822, 17967180, -9642269, -25402224,
                                47935224, -12355763, -61051912, 84172456, -3337400, -109622720,
                                115801768, 21078928, -160792432, 131305712, 57666468, -200080928,
                                124153440, 96339144, 1933618560, 96339144, 124153440, -200080928,
                                57666468, 131305712, -160792432, 21078928, 115801768, -109622720,
                                -3337400, 84172456, -61051912, -12355763, 47935224, -25402224,
                                -9642269, 17967180, -5721822, -2317085, -643, 1307894,
                                3494636, -6521107, 1715256, 5407067, -6182322, 450345,
                                4568168, -3931498, -371827, 3325511, -2424691 };
    iir_coeffs coeffs;
    prev_states prev_st;

    fileAdd = "C:/Filters/test_signal/noise.wav";
    newFileFIR = "C:/Filters/test_signal/out/fir.wav";
    newFileIIR = "C:/Filters/test_signal/out/iir.wav";

    if ((ptrWavFile = fopen(fileAdd, "rb")) == NULL)      /*  Open existance .wav file    */
    {
        printf("ERROR in opening existance file!\n");
    }

    if ((ptrNewWavFIR = fopen(newFileFIR, "w+b")) == NULL)      /*  Open existance .wav file    */
    {
        printf("ERROR in opening new file!\n");
    }

    if ((ptrNewWavIIR = fopen(newFileIIR, "w+b")) == NULL)      /*  Open existance .wav file    */
    {
        printf("ERROR in opening new file!\n");
    }

    readHeader(ptrRIFF, ptrFMT, ptrDATA, ptrWavFile, &numRead, &diviation);

    numWrite = fwrite(ptrRIFF, sizeof(uint8_t), sizeof(riffHeader), ptrNewWavFIR);
    numWrite = fwrite(ptrFMT, sizeof(uint8_t), sizeof(fmtHeader), ptrNewWavFIR);
    numWrite = fwrite(ptrDATA, sizeof(uint8_t), sizeof(dataHeader), ptrNewWavFIR);

    numWrite = fwrite(ptrRIFF, sizeof(uint8_t), sizeof(riffHeader), ptrNewWavIIR);
    numWrite = fwrite(ptrFMT, sizeof(uint8_t), sizeof(fmtHeader), ptrNewWavIIR);
    numWrite = fwrite(ptrDATA, sizeof(uint8_t), sizeof(dataHeader), ptrNewWavIIR);

    int samples = DATA.data_size / FMT.block_align;

    if (ptrFMT->format_type == 3)
    {
        long ofst = ptrFMT->length_of_fmt + 28 + diviation;
        size_t seek = fseek(ptrWavFile, ofst, SEEK_SET);

        //FIR_coeff_calc(h, ptrFMT->sample_rate, 0.3, 0.35);
        Init_CircBuff(buffer, ptrWavFile, &circ_Config, &numRead);
        FIR_tration(h, buffer, &circ_Config, ptrNewWavFIR, ptrWavFile, samples);
        
       seek = fseek(ptrWavFile, ofst, SEEK_SET);

        init_coeff_iir(&coeffs);
        init_states_iir(&prev_st);
        calc_coeffs(&coeffs, 15600.0, (float)FMT.sample_rate, 0.6);

        for (int i = 0; i < samples; i++)
        {
            iir_process(&coeffs, &prev_st, ptrWavFile, ptrNewWavIIR);
        }
    }
    else if (ptrFMT->format_type == 1)
    {
        long ofst = ptrFMT->length_of_fmt + 28 + diviation;
        size_t seek = fseek(ptrWavFile, ofst, SEEK_SET);

        Init_CircBuff_fxd(buffer_fxd, ptrWavFile, &circ_Config, &numRead);
        FIR_tration_fxd(h_fxd, buffer_fxd, &circ_Config, ptrNewWavFIR, ptrWavFile, samples);
    }
    else
    {
        printf("WRONG!\n");
    }



    int a = fclose(ptrWavFile);
    a = fclose(ptrNewWavIIR);
    a = fclose(ptrNewWavFIR);

    return 0;
}

void readHeader(riffHeader* ptrRIFF, fmtHeader* ptrFMT, dataHeader* ptrDATA, FILE* ptrWavFile, size_t* numRead, int* diviation)
{
    size_t offset;
    int fseek_ret;

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
        
    offset = ptrFMT->length_of_fmt + 20;
    fseek_ret = fseek(ptrWavFile, offset, SEEK_SET);

    numRead = fread(ptrDATA, sizeof(uint8_t), sizeof(dataHeader), ptrWavFile);
    if (numRead == sizeof(dataHeader))
    {
        printf("ALL DATA HEADER READED : %d from %d!\n", numRead, sizeof(dataHeader));
    }

    ptrRIFF->overall_size = ptrDATA->data_size + ptrFMT->length_of_fmt + 28;

    if (ptrFMT->length_of_fmt != 16)
    {
        *diviation = ptrFMT->length_of_fmt - 16;
        ptrFMT->length_of_fmt = 16;
    }

}

void create_new_pcm(riffHeader* ptrRIFF, fmtHeader* ptrFMT, dataHeader* ptrDATA, FILE* ptrWavFile, int diviation)
{
    FILE pcm;
    FILE* ptrPCM = &pcm;
    size_t numWrite;
    size_t seek;
    char fmt_pcm = 1;
    ptrPCM = fopen("C:/Filters/test_signal/pcm.wav", "w+b");

    numWrite = fwrite(ptrRIFF, sizeof(uint8_t), sizeof(riffHeader), ptrPCM);
    numWrite = fwrite(ptrFMT, sizeof(uint8_t), sizeof(fmtHeader), ptrPCM);
    numWrite = fwrite(ptrDATA, sizeof(uint8_t), sizeof(dataHeader), ptrPCM);

    seek = fseek(ptrPCM, 20, SEEK_SET);
    numWrite = fwrite(&fmt_pcm, 1, 1, ptrPCM);

    
    size_t offset = ptrFMT->length_of_fmt + 28;
    seek = fseek(ptrPCM, offset, SEEK_SET);

    offset = ptrFMT->length_of_fmt + 28 + diviation;
    seek = fseek(ptrWavFile, offset, SEEK_SET);

    size_t data_len = ptrDATA->data_size / 4;
    convert_pcm(ptrWavFile, ptrPCM, data_len);

    int a = fclose(ptrPCM);
}

