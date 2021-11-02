#ifndef OPCODE_H
#define OPCODE_H

#include "assembler.h"

typedef struct
{
    const char* name;
    int type;
    void (*fPtr)(void);
} opcode_t;


void opcode_parser(void);

#endif