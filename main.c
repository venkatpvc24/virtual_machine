#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "LC.h"
#include "./include/add.h"
#include "utilities.h"


#ifndef PRINT_CODE
  #define PRINT_CODE 0
#endif


int main() {

    uint16_t data[] = {0x4801, 0xd000, 0x903f, 0x1022, 0x1620, 0xc1c0};

    int ip = 0x3000;

    for (int i = 0; i < 6; i++) {
        __stack[ip + i] = data[i];
    }

    bool running = true;

    while (running) {
        uint16_t code = __stack[ip++];
        int op_code = code >> 12;

        switch (op_code) {
            case ADD: {
              add_t* a = decode_add(code);
              if (a->mode)
              {
                __stack[a->dst_r] = a->src_r1 + a->imme;
              }
              else
              {
                  __stack[a->dst_r] = a->src_r1 + a->src_r2;
              }
              printf("debug_lc-> %d\n", PRINT_CODE);
              if (PRINT_CODE == 1) print_add(a);
              update_flags(a->dst_r);

            }
            break;
            case AND: {
                printf("AND with 0x%x\n", code);
                reg_t dest = (code >> 9) & 0x07;
                reg_t r0 = (code >> 6) & 0x07;

                bool mode = code & (code << 5);

                if (mode) {
                    uint16_t val = code & 0x1F;
                    __stack[dest] = r0 & val;
                } else {
                    reg_t r1 = code & 0x1F;
                    __stack[dest] = __stack[r0] & __stack[r1];
                }

                printf("total: %d\n", __stack[dest]);
            } break;
            case NOT: {
                printf("i am hear in NOT with 0x%x\n", code);
                reg_t dest = (code >> 9) & 0x07;
                reg_t r2 = (code >> 6) & 0x07;

                __stack[dest] = ~__stack[r2];

                // update_flags(dest);

                printf("total: %d\n", __stack[dest]);
            } break;
            case LD: {
                reg_t dest = (code >> 9) & 0x07;
                uint16_t address = sign_extend((code & 0x1FF), 9);
                __stack[dest] = __stack[ip + address];
                printf("dest-> 0x%04x, data-> 0x%04x\n", dest, __stack[dest]);
            } break;
            case LDI: {
                printf("LDA\n\n\n");
                reg_t dest = (code >> 9) & 0x07;
                uint16_t address = sign_extend((code & 0x1FF), 9);
                address = __stack[ip + address];
                __stack[dest] = __stack[address];
                printf("0x%04x\n", __stack[dest]);
            } break;
                // setcc
            case LDR: {
                reg_t dest = (code >> 9) & 0x07;
                reg_t base = (code >> 6) & 0x07;

                uint16_t address = sign_extend((code & 0x3F), 6);

                __stack[dest] = __stack[__stack[base] + address];
                printf("dest-> 0x%04x, address-> 0x%04x, __stack->0x%04x\n",
                       dest, ip + address, __stack[dest]);

            }
            // setcc
            break;
            case LEA: {
                reg_t dest = (code >> 9) & 0x07;
                uint16_t address = sign_extend((code & 0x1FF), 9);
                __stack[dest] = ip + address;
                printf("dest-> 0x%04x, address-> 0x%04x\n", dest,
                       __stack[dest]);
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
                int test = __stack[R3];
            } break;
            case ST: {
                reg_t r0 = (code >> 9) & 0x07;
                uint16_t address = sign_extend(code & 0x1ff, 9);
                __stack[ip + (address)] = __stack[r0];
            } break;
            case STI: {
                reg_t r0 = (code >> 9) & 0x07;
                uint16_t address = sign_extend(code & 0x1ff, 9);
                __stack[__stack[ip + (address)]] = __stack[r0];

            } break;
            case STR: {
                reg_t dest = (code >> 9) & 0x07;
                reg_t base = (code >> 6) & 0x07;

                uint16_t address = sign_extend((code & 0x3F), 6);

                __stack[dest] = __stack[__stack[base] + address];

            } break;
            case RES:
            default: {
                running = false;
                printf("illegal op code\n");
            }
        }
    }

    return 0;

    // 01010000000001
}
