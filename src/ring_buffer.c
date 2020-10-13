#include "ring_buffer.h"

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

    for (i = 1; i < 65; i++)
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