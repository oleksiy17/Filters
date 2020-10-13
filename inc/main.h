#ifndef __MAIN_H_
#define __MAIN_H_

#include "stdlib.h"
#include "stdio.h"
#include "stdint.h"

#include "ring_buffer.h"
#include "fir_float.h"


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

void readHeader(riffHeader* ptrRIFF, fmtHeader* ptrFMT, dataHeader* ptrDATA, FILE* ptrWavFile, size_t* numRead);

#endif // !__MAIN_H_



