#include "float_to_pcm.h"
#include "fixedpoint.h"

void convert_pcm(FILE* source, FILE* destination, size_t data)
{
    size_t numRead;
    size_t numWrite;

    float buf;
    my_sint32 val;

    for (int i = 0; i < data; i++)
    {
        numRead = fread(&buf, sizeof(buf), 1, source);
        val = float_To_Fixed(buf, FRACTION_BASE);
        numWrite = fwrite(&val, sizeof(my_sint32), 1, destination);
    }
    
}