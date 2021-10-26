#ifndef PARSER_H
#define PARSER_H


#include <stdint.h>

#define LBL_SIZE 20

/*

Label only accpets 20 chars at this moment; you can change -DLBL_SIZE

statements must end with ';' like in c;

you can add your comment after semicolon

*/

#define VM_OPCODE 1
#define VM_LABEL 2
#define VM_PESUDO 3
#define VM_TRAP 4


// static const char* pesudo_codes[5] = {".orig", ".end", ".fill", ".blkw", ".stringz"};
typedef enum {
    state_br,
    state_add,
    state_ld,
    state_st,
    state_jsr,
    state_and,
    state_ldr,
    state_str,
    state_rti,
    state_not,
    state_ldi,
    state_sti,
    state_ret,
    state_res,
    state_lea,
    state_trap,
    state_pesudo,
    state_first_register,
    state_second_register,
    state_third_register,
    state_get_br_token

} opcodes_t;

typedef enum {
    state_error,
    state_next_line,
    state_start,
    state_start_origin_parser,
    state_start_label_parser,
    state_start_opcode_parser,
    state_start_trap_parser
} state;

typedef enum { start_pesudo_parser, start_p_value_parser } pesudo_t;

typedef struct{
  char* label;
  uint16_t address;
} label_t;

typedef struct {
    int type;
    char* origin;
    char* name;
    char* dest;
    char* sr1;
    char* sr2;
    char* pesudo;
    char* p_value;
    char* trap;
    label_t* label;
} vm_t;

struct node {
    vm_t* data;
    struct node* next;
};

typedef struct node node_t;

node_t* parser(const char* filename);
int get_op(char* str);

#endif
