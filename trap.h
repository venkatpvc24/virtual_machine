#ifndef TRAP_H
#define TRAP_H

#include "assembler.h"


typedef struct
{
    char* name;
    u16 code;
} trap_t;


void trap_parser(void);

#endif