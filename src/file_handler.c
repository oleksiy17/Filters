#include "file_handler.h"

ptrFile audio_file_open(void)
{
    FILE CreateFile;
    ptrFile openFiles;

    openFiles.wavRead = fopen("C:/Filters/test_signal/a.wav", "rb");
    openFiles.wavWrite = fopen("C:/Filters/test_signal/out/fir.wav", "w+b");

}

