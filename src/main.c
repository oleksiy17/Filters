#include "main.h"
#include "cJSON.h"
#include "file_handler.h"

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

    fileAdd = "C:/Filters/test_signal/in_sweep.wav";
    newFileFIR = "C:/Filters/test_signal/out/fir.wav";

    ptrWavFile = fopen(fileAdd, "rb");      // Open existance .wav file    
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

    effect_par_comp.fmt = ptrFMT; // for compressor
    effect_par_comp.data = ptrDATA;
    effect_par_comp.audio = audio;
    effect_par_comp.threshold = -6;
    effect_par_comp.ratio = 40;
    effect_par_comp.tauAttack = 30;
    effect_par_comp.tauRelease = 70;
    effect_par_comp.makeUpGain = 0;
    effect_par_comp.envAtt = 0;
    effect_par_comp.envRel = 30;

    numRead = fread(audio, 1, DATA.data_size, ptrWavFile);


    effect_params_equalizer eqzr;
    effect_params_equalizer *ptrEQZR;

  
    eqzr = (effect_params_equalizer){
        {   (equalizer) {20, -3, 1, 3},
            (equalizer) { 250, 0, 0.5, 5 },
            (equalizer) { 500, -3, 0.5, 2 },
            (equalizer) { 1000, -3, 4, 2 },
            (equalizer) { 2000, -3, 20, 2 },
            (equalizer) { 4000, -3, 4, 2 },
            (equalizer) { 8000, -3, 10, 2 },
            (equalizer) { 12000, -3, 8, 2 },
            (equalizer) { 14000, -3, 0.1, 4 },
            (equalizer) { 18000, -12, 1, 0 }},
        (float) 48000,
        ptrFMT,
        ptrDATA,
        audio
    };

    effect_params_crossover cross;
    effect_params_crossover* ptrCross = &cross;

    cross = (effect_params_crossover){ 100.0, 1000.0, 10000.0, ptrFMT, ptrDATA, audio};

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

#ifdef EQ
    effect_equalizer(eqzr);
#endif // Equalizer

#ifdef CROSS
    effect_crossover(cross);
#endif // Equalizer

    numWrite = fwrite(audio, 1, DATA.data_size, ptrNewWavFIR);

    int c = fclose(ptrWavFile);
    c = fclose(ptrNewWavFIR);
    
       return 0;
}


void effect_crossover(effect_params_crossover cross)
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

    effect_control_initialize(params, coeffs, cross.fmt->sample_rate);
    effect_reset(coeffs, states);

    effect_set_parameter(params, 0, cross.cutoff_1);
    effect_set_parameter(params, 1, cross.cutoff_2);
    effect_set_parameter(params, 2, cross.cutoff_3);
    effect_set_parameter(params, 3, cross.fmt->sample_rate);

    effect_update_coeffs(params, coeffs);

    effect_process(coeffs, states, cross.audio, (cross.data->data_size / cross.fmt->block_align));
}



void effect_equalizer(effect_params_equalizer eqzr)
{
    size_t params_bytes;
    size_t coeffs_bytes;
    size_t states_bytes;

    size_t i;
    size_t band;
    size_t param_num;

    void* params;
    void* coeffs;
    void* states;

    effect_control_get_sizes(&params_bytes, &coeffs_bytes);
    effect_process_get_sizes(&states_bytes);

    params = malloc(params_bytes);            // empty
    coeffs = malloc(coeffs_bytes);            // for h
    states = malloc(states_bytes);            // for circular buffer

    effect_control_initialize(params, coeffs, eqzr.sampleRate);
    effect_reset(coeffs, states);

    for (i = 0; i < 39; i++)
    {
        band = i >> 2;
        param_num = i - (4 * band);

        switch (param_num)
        {
            case 0:
            {
                effect_set_parameter(params, i, eqzr.eq[band].freq);
                break;
            }

            case 1:
            {
                effect_set_parameter(params, i, eqzr.eq[band].gain);
                break;
            }

            case 2:
            {
                effect_set_parameter(params, i, eqzr.eq[band].Q);
                break;
            }

            case 3:
            {
                effect_set_parameter(params, i, eqzr.eq[band].type);
                break;
            }

            default:
                break;
        }
    }

    effect_set_parameter(params, 40, eqzr.sampleRate);

    effect_update_coeffs(params, coeffs);

    effect_process(coeffs, states, eqzr.audio, (eqzr.data->data_size / eqzr.fmt->block_align));
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

