#ifndef  LC3_H
#define LC3_H


#define GENERATE_STR(S) #S

uint16_t __stack[UINT16_MAX];

typedef enum { R0, R1, R2, R3, R4, R5, R6, R7, R_PC, R_COND, R_COUNT } reg_t;

typedef enum {
    BR = 0b0000,
    ADD = 0b0001,
    LD = 0b0010,
    ST = 0b0011,
    JSR = 0b0100,
    AND = 0b0101,
    LDR = 0b0110,
    STR = 0b0111,
    RTI = 0b1000,
    NOT = 0b1001,
    LDI = 0b1010,
    STI = 0b1011,
    RET = 0b1100,
    RES = 0b1101,
    LEA = 0b1110,
    TRAP = 0b1111
} opcode_t;

typedef enum { F_POS = 1, F_ZERO = 2, F_NEG = 3 } cond_flag_t;

#endif
