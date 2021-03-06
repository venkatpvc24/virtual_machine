#ifndef VM_H
#define VM_H

#include <stdint.h>

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

typedef enum {
    GETC = 0x20,
    OUT = 0x21,
    PUTS = 0x22,
    IN = 0x23,
    HALT = 0x25
} traps_t;


typedef enum { R0, R1, R2, R3, R4, R5, R6, R7, R_PC, R_COND, R_COUNT } reg_t;

typedef enum { F_POS = 1, F_ZERO = 2, F_NEG = 3 } cond_flag_t;

int execute_instructions(uint16_t* data, uint16_t address);

#endif
