#include "../include/trap.h"
#include "../include/assembler.h"

extern u16 line_number, size;
extern u16 data[USHRT_MAX];
extern int __require;
extern bool base10, base16, is_reg;
extern const int traps[];

extern label_t* labels;

void trap_parser(void)
{

    u16 codes[5] = {0xF020, 0xF021, 0xF022, 0xF023, 0xF025};

    for (int i = 0; i < 5; i++)
    {
        if (vm_string_hashing(parser.opcode) == traps[i]) 
        {
            data[size++] = codes[i];
            break;
        }
    }

    line_number++;

}