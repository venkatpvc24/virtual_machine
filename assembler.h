#ifndef ASSEMBLER_H
#define ASSEMBLER_H


#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef uint16_t u16;
typedef char* cPtr;


typedef enum {
    TK_START,
    TK_COMMA,
    TK_SPACE,
    TK_SEMICOLON,
    TK_QUOTES,
    TK_DOT,
    TK_ALPHA,
    TK_NUMBER,
    TK_EOL
} vm_state_t;

typedef enum { TYPE_ORIG, TYPE_OPCODE, TYPE_TRAP, TYPE_LABEL, TYPE_START_END } vm_types_t;
typedef enum
{
    VM_BR,
    VM_ADD,
    VM_LD,
    VM_ST,
    VM_JSR,
    VM_AND,
    VM_LDR,
    VM_STR,
    VM_RTI,
    VM_NOT,
    VM_LDI,
    VM_STI,
    VM_RET,
    VM_RES,
    VM_LEA,
    VM_TRAP,
    VM_START,
    VM_EOL = -1
} vm_opcode_t;

typedef struct {
    char* current;
    int count;
} vm_token_t;

typedef struct {
    cPtr name;
    int line_number;
} label_t;

typedef struct {
    int type;
    int current;
    cPtr opcode;
} op_trap_t;



#define TO_NUMBER(reg) (reg[1] - '0')

void assembler(const char* filename, u16* data, u16* len_of_data, u16* start_addres);

#endif

//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
