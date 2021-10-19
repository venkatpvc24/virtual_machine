#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

// #include "LC.h"
// #include "utilities.h"

#ifdef TRACE
    #define DEBUG_TRACE(...) (fprintf(stderr, __VA_ARGS__))
#else
    #define DEBUG_TRACE(...)
#endif


typedef enum {
    BR,
    ADD,
    LD,
    ST,
    JSR,
    AND,
    LDR,
    STR,
    RTI,
    NOT,
    LDI,
    STI,
    RET,
    RES,
    LEA,
    TRAP
} opcode_t;


uint16_t __stack[UINT16_MAX];

typedef enum { R0, R1, R2, R3, R4, R5, R6, R7, R_PC, R_COND, R_COUNT } reg_t;

typedef enum { F_POS = 1, F_ZERO = 2, F_NEG = 3 } cond_flag_t;

void update_flags(uint16_t r)
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

uint16_t sign_extend(uint16_t x, int bit_count)
{
    if ((x >> (bit_count - 1)) & 1) {
        x |= (0xFFFF << bit_count);
    }
    return x;
}

int execute_instructions(uint16_t data, uint16_t size, start_address)

    memset(__stack, 0, sizeof __stack);

    uint16_t ip = start_address;

    for (int i = 0; i < size; i++) {
        __stack[ip + i] = data[i];
    }

    bool running = true;

    while (running) {
        uint16_t code = __stack[ip++];
        int op_code = code >> 12;

        switch (op_code) {
            case ADD: {
                reg_t dest = (code >> 9) & 0x7;
                reg_t sr1 = (code >> 6) & 0x7;
                bool mode = (code >> 5) & 0x1;

                if (mode) {
                    uint16_t val = sign_extend((code & 0x1F), 5);
                    DEBUG_TRACE("OP_CODE_ADD dr: 0x%04x sr1: 0x%04x imme: 0x%04x\n", dest, sr1, val);
                    __stack[dest] = __stack[sr1] + val;
                } else {
                    reg_t sr2 = code & 0x07;
                    DEBUG_TRACE("OP_CODE_ADD dr: 0x%04x sr1: 0x%04x sr2: 0x%04x\n", dest, sr1, sr2);
                    __stack[dest] = __stack[sr1] + __stack[sr2];
                }
                update_flags(dest);

            } break;
            case AND: {
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

            } break;
            case NOT: {
                reg_t dest = (code >> 9) & 0x07;
                reg_t sr1 = (code >> 6) & 0x07;
                DEBUG_TRACE("OP_CODE_NOT dr: 0x%04x sr1: 0x%04x\n", dest, sr1);
                __stack[dest] = ~__stack[sr1];

                update_flags(dest);
            } break;
            case LD: {
                reg_t dest = (code >> 9) & 0x07;
                uint16_t address = ip + sign_extend((code & 0x1FF), 9);
                DEBUG_TRACE("OP_CODE_NOT dr: 0x%04x address: 0x%04x\n", dest, address);
                __stack[dest] = __stack[address];
            } break;
            case LDI: {
                reg_t dest = (code >> 9) & 0x07;
                uint16_t address = ip + sign_extend((code & 0x1FF), 9);
                DEBUG_TRACE("OP_CODE_NOT dr: 0x%04x address: 0x%04x\n", dest, address);
                address = __stack[address];
                __stack[dest] = __stack[address];
            } break;
                // setcc
            case LDR: {
                reg_t dest = (code >> 9) & 0x07;
                reg_t base = (code >> 6) & 0x07;

                DEBUG_TRACE("OP_CODE_NOT dr: 0x%04x address: 0x%04x\n", dest, base);

                uint16_t address = sign_extend((code & 0x3F), 6);

                __stack[dest] = __stack[__stack[base] + address];

            }
            // setcc
            break;
            case LEA: {
                reg_t dest = (code >> 9) & 0x07;
                uint16_t address = sign_extend((code & 0x1FF), 9);
                DEBUG_TRACE("OP_CODE_LEA dr: 0x%04x address: 0x%04x\n", dest, address);
                __stack[dest] = ip + address;
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
                printf("%d\n", __stack[R3]);
            }
        }
    }
    return 0;
    // 01010000000001
}
