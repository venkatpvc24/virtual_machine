#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include <stdbool.h>
#include <stdint.h>


#define INVALID_REG 1
#define INVALID_OP 2
#define INVALID_NUM_SYNTAX 3
#define OUT_OF_RANGE_IMME5 4
#define INVALID_EXPRESSION 5
#define OUT_OF_RANGE_IMME9 6
#define VM_SUCESS 7
#define VM_VALID_NUMBER 8

#ifdef TRACE
    #define DEBUG_TRACE(...) (fprintf(stderr, __VA_ARGS__))
#else
    #define DEBUG_TRACE(...)
#endif


#define TO_INT(reg) (reg[1] - '0')

#define CHECK_REG(reg) ((reg[0] == 'r' || reg[0] == 'R') && TO_INT(reg) >= 0 && TO_INT(reg) <= 7)

#define IS_OFFSET5(num) ( (num >= -16 && num <= 15))

#define IS_OFFSET9(num) ( (num >= -256 && num <= 255))



static const char* arr[17] = {"br", "add", "ld", "st", "jsr", "and", "ldr",
                  "str", "rti", "not", "ldi", "sti", "ret", "res", "lea", "trap", "pesudo"};


/*
    struct for counting arguments and record error
*/

typedef struct
{
    int error;
    int count;
} args_t;


/*
    struct to find imme or register and number
*/


typedef struct
{
    bool imme;
    bool reg;
    int value;
} imme_reg_t;



void assembler(node_t** node, uint16_t* output, uint16_t* size, uint16_t* start_address);


#endif
