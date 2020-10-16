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

    (float*)circ_Config->begin = &buf[0];
    (float*)circ_Config->end = &buf[64];
    (float*)circ_Config->current = &buf[0];

    for (i = 1; i < 65; i++)
    {
        buf[i] = 0.0;
    }
}


void SetCurrent_CircBuff(CircularBuffer* circ_Config)
{
    if ((float*)circ_Config->current == (float*)circ_Config->begin)
    {
        (float*)circ_Config->current = (float*)circ_Config->end;
    }
    else
    {
        ((float*)circ_Config->current)--;
    }
}

void Convolution_CircBuff(CircularBuffer* circ_Config)
{
    if ((float*)circ_Config->current == (float*)circ_Config->end)
    {
        (float*)circ_Config->current = (float*)circ_Config->begin;
    }
    else
    {
        ((float*)circ_Config->current)++;
    }
}

void Update_CircBuff(float* buf, FILE* readFile, CircularBuffer* circ_Config)
{
    int seek;
    float val;
    float* ptrVal; //float* ptrVal;
    size_t numRead;

    val = 0.0;
    ptrVal = &val;

    SetCurrent_CircBuff(circ_Config);   //set current posuition for re-writting
    numRead = fread(ptrVal, sizeof(float), 1, readFile);
    seek = fseek(readFile, sizeof(float), SEEK_CUR);

    *((float*)(circ_Config->current)) = val;
}


void Init_CircBuff_fxd(my_sint32* buf, FILE* readFile, CircularBuffer* circ_Config, size_t* numRead)
{
    int i;
    int seek;
    my_sint32 val;
    my_sint32* ptrVal;

    val = 0;
    ptrVal = &val;

    numRead = fread((my_sint32*)ptrVal, (size_t)sizeof(my_sint32), (size_t)1, (FILE*)readFile);
    seek = fseek(readFile, sizeof(my_sint32), SEEK_CUR);
    buf[0] = val;

    (my_sint32*)circ_Config->begin = &buf[0];
    (my_sint32*)circ_Config->end = &buf[64];
    (my_sint32*)circ_Config->current = &buf[0];

    for (i = 1; i < 65; i++)
    {
        buf[i] = 0;
    }
}

void Update_CircBuff_fxd(my_sint32* buf, FILE* readFile, CircularBuffer* circ_Config)
{
    int seek;
    my_sint32 val;
    my_sint32* ptrVal; //float* ptrVal;
    size_t numRead;

    val = 0;
    ptrVal = &val;

    SetCurrent_CircBuff_fxd(circ_Config);   //set current posuition for re-writting
    numRead = fread(ptrVal, sizeof(my_sint32), 1, readFile);
    seek = fseek(readFile, sizeof(my_sint32), SEEK_CUR);

    *((my_sint32*)(circ_Config->current)) = val;
}

void SetCurrent_CircBuff_fxd(CircularBuffer* circ_Config)
{
    if ((my_sint32*)circ_Config->current == (my_sint32*)circ_Config->begin)
    {
        (my_sint32*)circ_Config->current = (my_sint32*)circ_Config->end;
    }
    else
    {
        ((my_sint32*)circ_Config->current)--;
    }
}

void Convolution_CircBuff_fxd(CircularBuffer* circ_Config)
{
    if ((my_sint32*)circ_Config->current == (my_sint32*)circ_Config->end)
    {
        (my_sint32*)circ_Config->current = (my_sint32*)circ_Config->begin;
    }
    else
    {
        ((my_sint32*)circ_Config->current)++;
    }
}