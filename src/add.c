
#include "../include/add.h"
#include "../utilities.h"
#include "../LC.h"


add_t* decode_add(uint16_t byte)
{

    add_t* a = (add_t*)malloc(sizeof *a);

    a->op_code = byte >> 12;
    a->dst_r = (byte >> 9) & 0x07;
    a->src_r1 = (byte >> 6) & 0x07;
    a->mode = byte & ( byte << 5);

    if (a->mode)
    {
        a->imme = sign_extend(byte & 0x1F, 5);
    }
    else
    {
        a->src_r2 = byte & 0x07;
    }

    return a;
}


void print_add(add_t* a)
{
    //printf("%s %s %s\n", opcode_to_str(a->op_code), reg_to_str(a->dst_r), reg_to_str(a->src_r1));
    printf("********** add operation ************\n\n");
    printf("op_code:             %d\n", a->op_code);
    printf("destinaion register: %d\n", a->dst_r);
    printf("source register_1:   %d\n", a->src_r1);
    printf("mode:                %d\n", a->mode);
    printf("source register_2:   %d\n", a->src_r2);
    printf("\n\n");

}
