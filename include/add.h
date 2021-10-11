#ifndef ADD_H
#define ADD_H


#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>


typedef struct
{
    uint16_t op_code;
    uint16_t dst_r; // destination registor
    uint16_t src_r1; // source register one
    bool mode;
    uint16_t src_r2; // source regiter two
    uint16_t imme;
} add_t;


add_t* decode_add(uint16_t byte);
void print_add(add_t* a);


#endif
