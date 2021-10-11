

#include "../include/and.h"


add_t* decode_and(uint16_t byte)
{

    and_t* a = (and_t*)malloc(sizeof *a);

    a->op_code = byte >> 12;
    a->dst_r = (byte >> 9) & 0x07;
    a->src_r1 = (byte >> 6) & 0x07;
    a->mode = byte & ( byte << 5);

    if (a->mode)
    {
        a->src_r2 = byte & 0x1F;
    }
    else
    {
        a->src_r2 = byte & 0x07;
    }

    return a;
}


void print_and(and_t* a)
{
    printf("********** add operation ************\n\n");
    printf("op_code:             %d\n", a->op_code);
    printf("destinaion register: %d\n", a->dst_r);
    printf("source register_1:   %d\n", a->src_r1);
    printf("mode:                %d\n", a->mode);
    printf("source register_2:   %d\n", a->src_r2);
    printf("\n\n");
}


bool op_and(int r1, int r2)
{
    return r1 & r2;
}
