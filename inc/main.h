#include "stdlib.h"
#include "stdio.h"
#include "stdint.h"
#include "math.h"

#define M2_PI 6.283185307179
#define TAP_64 65

typedef struct {
    uint8_t    riff[4];                    // RIFF string
    uint32_t   overall_size;  	            // overall size of file in bytes
    uint8_t    wave[4];                    // WAVE string
} riffHeader;

typedef struct {
    uint8_t    fmt_chunk_marker[4];        // fmt string with trailing null char
    uint32_t   length_of_fmt;              // length of the format data
    uint16_t   format_type;                // format type
    uint16_t   channels;                   // # of channels
    uint32_t   sample_rate;                // sampling rate 
    uint32_t   byterate;                   // SampleRate * NumChannels * BitsPerSample/8
    uint16_t   block_align;                // NumChannels * BitsPerSample/8
    uint16_t   bits_per_sample;            // bits per sample, 8- 8bits, 16- 16 bits
}fmtHeader;

typedef struct {
    uint8_t    data_chunk_header[4];           // DATA string or FLLR string
    uint32_t   data_size;                  // NumSamples * NumChannels * BitsPerSample/8 - size of the next chunk that will be read
}dataHeader;

typedef struct {
    float* current;
    float* last;
    float* begin;
    float* end;
}CircularBuffer;

void readHeader(riffHeader* ptrRIFF, fmtHeader* ptrFMT, dataHeader* ptrDATA, FILE* ptrWavFile, size_t* numRead);
void writeHeader(riffHeader* ptrRIFF, fmtHeader* ptrFMT, dataHeader* ptrDATA, FILE* ptrNewWavFile, size_t* numWrite);
void FIR_coeff_calc(float* h, float sampRate, float cof1, float cof2);

void Init_CircBuff(float* buf, FILE* readFile, CircularBuffer* circ_Config, size_t* numRead);
void Update_CircBuff(float* buf, FILE* readFile, CircularBuffer* circ_Config);

void FIR_tration(float* h, float* buffer, CircularBuffer* circ_Config);
