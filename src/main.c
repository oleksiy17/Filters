#include "main.h"

int main()
{
    static FILE* ptrWavFile;

    FILE newWavFIR;
    FILE* ptrNewWavFIR = &newWavFIR;

    char* fileAdd;
    char* newFileFIR;
    char* newFileIIR;
    int diviation = 0;

    size_t numRead = 0;
    size_t numWrite = 0;

    riffHeader RIFF;
    riffHeader* ptrRIFF = &RIFF;
    fmtHeader FMT;
    fmtHeader* ptrFMT = &FMT; 
    dataHeader DATA;
    dataHeader* ptrDATA = &DATA;

    effect_parameters effect_params;
    effect_params_compressor effect_par_comp;

    fileAdd = "C:/Filters/test_signal/in_level_pcm.wav";
    newFileFIR = "C:/Filters/test_signal/out/fir.wav";

    ptrWavFile = fopen(fileAdd, "rb");     /*  Open existance .wav file    */
    ptrNewWavFIR = fopen(newFileFIR, "w+b");      

    readHeader(ptrRIFF, ptrFMT, ptrDATA, ptrWavFile, &numRead, &diviation);

    numWrite = fwrite(ptrRIFF, sizeof(uint8_t), sizeof(riffHeader), ptrNewWavFIR);
    numWrite = fwrite(ptrFMT, sizeof(uint8_t), sizeof(fmtHeader), ptrNewWavFIR);
    numWrite = fwrite(ptrDATA, sizeof(uint8_t), sizeof(dataHeader), ptrNewWavFIR);

    size_t audio_bytes;
    audio_bytes = DATA.data_size;
    void* audio = malloc(audio_bytes);

    size_t samples_count = DATA.data_size / FMT.block_align;

    effect_params.fmt = ptrFMT;
    effect_params.data = ptrDATA;
    effect_params.audio = audio;
    effect_params.Q = 0.5;
    effect_params.freq = 15600.0;
    effect_params.gain = 18.0;

    /*effect_par_comp.fmt = ptrFMT; // for compressor
    effect_par_comp.data = ptrDATA;
    effect_par_comp.audio = audio;
    effect_par_comp.threshold = -18;
    effect_par_comp.ratio = 40;
    effect_par_comp.tauAttack = 70;
    effect_par_comp.tauRelease = 300;
    effect_par_comp.makeUpGain = 0;

    effect_par_comp.envAtt = 0;
    effect_par_comp.envRel = 50;*/

    effect_par_comp.fmt = ptrFMT; // for compressor
    effect_par_comp.data = ptrDATA;
    effect_par_comp.audio = audio;
    effect_par_comp.threshold = -2;
    effect_par_comp.ratio = 40;
    effect_par_comp.tauAttack = 30;
    effect_par_comp.tauRelease = 100;
    effect_par_comp.makeUpGain = 0;

    effect_par_comp.envAtt = 0;
    effect_par_comp.envRel = 50;

    numRead = fread(audio, 1, DATA.data_size, ptrWavFile);

#ifdef FIR_float
    effect_fir(effect_params);
#endif // FIR

#ifdef IIR_float
    effect_iir(effect_params);
#endif // IIR_float

#ifdef GAIN
    effect_gain(effect_params);
#endif // GAIN

#ifdef COMPRESSOR
    effect_compressor(effect_par_comp);
#endif // COMPRESSOR

    numWrite = fwrite(audio, 1, DATA.data_size, ptrNewWavFIR);

    int c = fclose(ptrWavFile);
    c = fclose(ptrNewWavFIR);
    
    float e = 0.50141;
    float r = 0.975;
    
    
    printf("pow %f^%f = %f\n", e, r,  pow(e,r));

    my_sint32 k = float_To_Fixed(e, Q31);
    my_sint32 l = float_To_Fixed(r, Q31);
    printf("%d %d\n", k, l);
    printf("powx %f^%f = %f\n", e, r,  fixed_To_Float(my_pow(k, l), Q31));



    /*for (int q = 0; q < 52; q++)
    {
        for (int w = 0; w < 10; w++)
        {
            printf("%d, ", float_To_Fixed(log2(e), Q27));
            e += 1.0 / 512.0;
        }

        printf("\n");

    }*/


    /*e = 0.00316;
    printf("log2 %f = %f\n", e, log2f(e));

    my_sint32 k = float_To_Fixed(e, Q31);
    printf("log2x %f = %f\n", e, fixed_To_Float(log2x(k), Q26));*/
    
       return 0;
}

void effect_compressor(effect_params_compressor effect_par_comp)
{
    size_t params_bytes;
    size_t coeffs_bytes;
    size_t states_bytes;

    void* params;
    void* coeffs;
    void* states;

    effect_control_get_sizes(&params_bytes, &coeffs_bytes);
    effect_process_get_sizes(&states_bytes);

    params = malloc(params_bytes);            // empty
    coeffs = malloc(coeffs_bytes);            // for h
    states = malloc(states_bytes);            // for circular buffer


    effect_control_initialize(params, coeffs, effect_par_comp.fmt->sample_rate);
    effect_reset(coeffs, states);

    effect_set_parameter(params, 0, effect_par_comp.threshold);
    effect_set_parameter(params, 1, effect_par_comp.ratio);
    effect_set_parameter(params, 2, effect_par_comp.tauAttack);
    effect_set_parameter(params, 3, effect_par_comp.tauRelease);
    effect_set_parameter(params, 4, effect_par_comp.makeUpGain);
    effect_set_parameter(params, 5, effect_par_comp.fmt->sample_rate);
    effect_set_parameter(params, 6, effect_par_comp.envAtt);
    effect_set_parameter(params, 7, effect_par_comp.envRel);

    effect_update_coeffs(params, coeffs);

    effect_process(coeffs, states, effect_par_comp.audio, (effect_par_comp.data->data_size / effect_par_comp.fmt->block_align));
}

void effect_gain(effect_parameters effect_params)
{
    size_t params_bytes;
    size_t coeffs_bytes;
    size_t states_bytes;

    void* params;
    void* coeffs;
    void* states;

    effect_control_get_sizes(&params_bytes, &coeffs_bytes);
    effect_process_get_sizes(&states_bytes);

    params = malloc(params_bytes);            // empty
    coeffs = malloc(coeffs_bytes);            // for h
    states = malloc(states_bytes);            // for circular buffer


    effect_control_initialize(params, coeffs, effect_params.fmt->sample_rate);
    
    effect_set_parameter(params, 0, effect_params.gain);
    effect_update_coeffs(params, coeffs);

    effect_process(coeffs, states, effect_params.audio, (effect_params.data->data_size / effect_params.fmt->block_align));
}

void effect_fir(effect_parameters effect_params)
{
    size_t params_bytes;
    size_t coeffs_bytes;
    size_t states_bytes;

    void* params;        
    void* coeffs;     
    void* states;

    effect_control_get_sizes(&params_bytes, &coeffs_bytes);
    effect_process_get_sizes(&states_bytes);

    params = malloc(params_bytes);            // empty
    coeffs = malloc(coeffs_bytes);            // for h
    states = malloc(states_bytes);            // for circular buffer


    effect_control_initialize(params, coeffs, effect_params.fmt->sample_rate);
    effect_reset(coeffs, states);
    printf("coeffs[0] = %d, coeffs[1] = %d, coeffs[2] = %d\n", ((my_sint32*)coeffs)[0], ((my_sint32*)coeffs)[1], ((my_sint32*)coeffs)[2]);
    effect_process(coeffs, states, effect_params.audio, (effect_params.data->data_size/effect_params.fmt->block_align));
}

void effect_iir(effect_parameters effect_params)
{
    size_t params_bytes;
    size_t coeffs_bytes;
    size_t states_bytes;

    void* params;
    void* coeffs;
    void* states;

    effect_control_get_sizes(&params_bytes, &coeffs_bytes);
    effect_process_get_sizes(&states_bytes);

    params = malloc(params_bytes);            // empty
    coeffs = malloc(coeffs_bytes);            // for h
    states = malloc(states_bytes);            // for circular buffer


    effect_control_initialize(params, coeffs, effect_params.fmt->sample_rate);
    effect_reset(coeffs, states);

    printf("coeffs[0] = %d, coeffs[1] = %d, coeffs[2] = %d\n", ((my_sint32*)coeffs)[0], ((my_sint32*)coeffs)[1], ((my_sint32*)coeffs)[2]);

    effect_set_parameter(params, 0, effect_params.freq);
    effect_set_parameter(params, 1, effect_params.Q);
    effect_set_parameter(params, 2, effect_params.fmt->sample_rate);

    effect_update_coeffs(params, coeffs);

    effect_process(coeffs, states, effect_params.audio, (effect_params.data->data_size / effect_params.fmt->block_align));
}

void readHeader(riffHeader* ptrRIFF, fmtHeader* ptrFMT, dataHeader* ptrDATA, FILE* ptrWavFile, size_t* numRead, int* diviation)
{
    size_t offset;
    int fseek_ret;

    numRead = fread(ptrRIFF, sizeof(uint8_t), sizeof(riffHeader), ptrWavFile);
    numRead = fread(ptrFMT, sizeof(uint8_t), sizeof(fmtHeader), ptrWavFile);

    offset = ptrFMT->length_of_fmt + 20;
    fseek_ret = fseek(ptrWavFile, offset, SEEK_SET);

    numRead = fread(ptrDATA, sizeof(uint8_t), sizeof(dataHeader), ptrWavFile);

    ptrRIFF->overall_size = ptrDATA->data_size + ptrFMT->length_of_fmt + 28;

    if (ptrFMT->length_of_fmt != 16)
    {
        *diviation = ptrFMT->length_of_fmt - 16;
        ptrFMT->length_of_fmt = 16;
    }

}

