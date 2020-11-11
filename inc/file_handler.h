#ifndef __FILE_HANDLER_H_
#define __FILE_HANDLER_H_

#include "stdio.h"

typedef struct {
    FILE* wavRead;
    FILE* wavWrite;
}ptrFile;

ptrFile audio_file_open(void);

#endif // !__FILE_HANDLER_H_

