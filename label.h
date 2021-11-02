#ifndef LABEL_H
#define LABEL_H

#include "assembler.h"



typedef struct
{
    const char* name;
    int type;
    void (*fPtr)(void);
} pesudo_t;

void label_pesudo_parser(void);
void collect_labels(int index, FILE* in, char* line);

#endif