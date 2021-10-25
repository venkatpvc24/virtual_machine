#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// #include "LC.h"
// #include "utilities.h"

#include "vm.h"


static void update_flags(uint16_t r, uint16_t* __stack)
{
    if (__stack[r] == 0) {
        __stack[R_COND] = F_ZERO;
    } else if (__stack[r] >> 15)  // a 1 in the left-most bit indicates negative
    {
        __stack[R_COND] = F_NEG;
    } else {
        __stack[R_COND] = F_POS;
    }
}

static uint16_t sign_extend(uint16_t x, int bit_count)
{
    if ((x >> (bit_count - 1)) & 1) {
        x |= (0xFFFF << bit_count);
    }
    return x;
}


// execute_instructions(node_t* data, uint16_t size, uint16_t address);

int execute_instructions(uint16_t* data, uint16_t size, uint16_t start_address)
{

  uint16_t __stack[5000];

    memset(__stack, 0, sizeof __stack);

    uint16_t ip = start_address;
    printf("ip: 0x%04x\n", ip);

    for (int i = 0; i < size; i++) {
        __stack[ip + i] = data[i];
        //printf("code: 0x%04x, ", __stack[ip+i]);
    }

    bool running = true;
    char ch;
    while (running) {
        uint16_t code = __stack[ip++];
        int op_code = code >> 12;
        reg_t dest = (code >> 9) & 0x07;
        reg_t sr1 = (code >> 6) & 0x07;
        reg_t sr2 = code & 0x07;
        bool mode = (code >> 5) & 0x1;


        switch (op_code) {
            case ADD: {

                if (mode) {
                    uint16_t val = sign_extend((code & 0x1F), 5);
                    //DEBUG_TRACE("OP_CODE_ADD dr: 0x%04x sr1: 0x%04x imme: 0x%04x\n", dest, sr1, val);
                    __stack[dest] = __stack[sr1] + val;
                } else {
                    //DEBUG_TRACE("OP_CODE_ADD dr: 0x%04x sr1: 0x%04x sr2: 0x%04x\n", dest, sr1, sr2);
                    __stack[dest] = __stack[sr1] + __stack[sr2];
                }
                update_flags(dest, __stack);
            } break;
            case AND: {

                if (mode) {
                    uint16_t val = sign_extend(code & 0x1F, 5);
                    __stack[dest] = __stack[sr1] & val;
                } else {
                    __stack[dest] = __stack[sr1] & __stack[sr2];
                }
                update_flags(dest, __stack);
            } break;
            case NOT: {

                //DEBUG_TRACE("OP_CODE_NOT dr: 0x%04x sr1: 0x%04x\n", dest, sr1);
                __stack[dest] = ~__stack[sr1];
                update_flags(dest, __stack);
            } break;
            case LD: {
                uint16_t address = ip + sign_extend((code & 0x1FF), 9);
                //DEBUG_TRACE("OP_CODE_NOT dr: 0x%04x address: 0x%04x\n", dest, address);
                __stack[dest] = __stack[address];
                update_flags(dest, __stack);
            } break;
            case LDI: {
                uint16_t address = ip + sign_extend((code & 0x1FF), 9);
                //DEBUG_TRACE("OP_CODE_NOT dr: 0x%04x address: 0x%04x\n", dest, address);
                address = __stack[address];
                __stack[dest] = __stack[address];
                update_flags(dest, __stack);
            } break;
                // setcc
            case LDR: {
                //DEBUG_TRACE("OP_CODE_NOT dr: 0x%04x address: 0x%04x\n", dest, base);
                uint16_t address = sign_extend((code & 0x3F), 6);
                __stack[dest] = __stack[__stack[sr1] + address];
                update_flags(dest, __stack);
            }
            // setcc
            break;
            case LEA: {
                uint16_t address = sign_extend((code & 0x1FF), 9);
                //DEBUG_TRACE("OP_CODE_LEA dr: 0x%04x address: 0x%04x\n", dest, address);
                __stack[dest] = ip + address;
                update_flags(dest, __stack);
            }
            // setcc
            break;
            case BR: {
                int16_t address = sign_extend(code & 0x1ff, 9);
                uint16_t mode = (code >> 9) & 0x07;
                if (mode && __stack[R_COND] == 1) ip += (address);
                if (mode && __stack[R_COND] == 3) break;
            } break;
            case JSR: {
                uint16_t address = sign_extend( code & 0x7FF, 11);
                __stack[R7] = ip;
                ip = ip + address;
            }
            break;
            case RET: {
                ip = __stack[R7];
            } break;
            case ST: {
                uint16_t address = sign_extend(code & 0x1ff, 9);
                __stack[ip + (address)] = __stack[sr1];
            } break;
            case STI: {
                uint16_t address = sign_extend(code & 0x1ff, 9);
                __stack[__stack[ip + (address)]] = __stack[sr1];

            } break;
            case STR: {
                uint16_t address = sign_extend((code & 0x3F), 6);
                __stack[dest] = __stack[__stack[sr1] + address];
            } break;
            case TRAP:
            {
                uint16_t trap = code & 0x00FF;
                /*__stack[R7] = ip;
                ip = __stack[address]; */

                switch(trap)
                {
                    case GETC:
                    {
                        __stack[R0] = '1'; //(uint16_t)getchar();
                        break;
                    }
                    case OUT:
                    {
                        printf("%c", ch);
                        break;
                    }
                    case PUTS:
                    {
                        uint16_t address = __stack[R0];
                        while(__stack[address])
                        {
                            printf("%c", __stack[address++]);
                        }
                      }
                        break;
                    case IN:
                        printf("$ ");
                        __stack[0] = (uint16_t)getchar(); getchar();
                    case HALT:
                        exit(EXIT_SUCCESS);
                        break;
                    default:
                    {
                        printf("incorrect trap number\n");
                    }
                }
            }
            break;
            case RES:
            default: {
                running = false;
                printf("%d\n", __stack[R3]);
            }
        }
    }
    return 0;
    // 01010000000001
}


/*

  example run:

  ./vm -a "file path" ; to produce obj FILE
  ./vm -ar "file path" ; generates object file and runs prints results on console.

*/

/*
  currently support only decimal (base 10) numbers

  example: #10

  to-do
  -> NEED TO ADD, TO ACCEPT HEX AND BINARY NUMBERS

*/

/*

  results must be store on register R3 to print on console.

*/

/*

  to-do
  -> MEMORY MAPPED I/O ; to read and write
  -> TRAPS
  -> OS
  -> GUI SIMULATOR

*/
