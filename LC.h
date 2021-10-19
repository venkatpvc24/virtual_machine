#ifndef LC_H
#define LC_H


#define GENERATE_STR(S) #S

uint16_t __stack[UINT16_MAX];

typedef enum { R0, R1, R2, R3, R4, R5, R6, R7, R_PC, R_COND, R_COUNT } reg_t;

typedef enum {
    BR
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

typedef enum { F_POS = 1, F_ZERO = 2, F_NEG = 3 } cond_flag_t;

#endif
