#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LBL_SIZE 20

/*

Label only accpets 20 chars at this moment; you can change -DLBL_SIZE

statements must end with ';' like in c;

you can add your comment after semicolon

*/

static char currentToken[4096];
static size_t currentTokenLength;

static void add_token(char c) {
    // printf("token: %c", c);
    if (currentTokenLength < sizeof(currentToken)) {
        currentToken[currentTokenLength++] = c;
    }
}

char arr[100];

static void print_token(void) {
    sprintf(arr, "Token: >>>%.*s<<<\n", (int)currentTokenLength, currentToken);
    printf("%s", arr);
    currentTokenLength = 0;
}

typedef enum {
    state_error,
    state_start_origin,
    state_origin,
    state_get_memory_location,
    state_start_origin_parser,
    state_end_origin_parser,
    state_start_number_parser,
    state_end_number_parser,
    state_end_line_parser,
    state_start_add_parser,
    state_start_and_parser,
    state_end_add_parser,
    state_end_and_parser,
    state_start_comment_line,
    state_end_comment_line,
    state_add_register_first,
    state_add_register_second,
    state_add_register_third,
    state_add_register_end,
    state_and_register_first,
    state_and_register_second,
    state_and_register_third,
    state_next_line,
    state_start_label,
    state_end_label,
    state_start_br_parser,
    state_end_br_parser,
    state_get_br_token,
    state_start_trap,
    state_end_trap,
    state_start_lea_parser,
    state_start_ld_parser,
    state_ld_register_first,
    state_ld_register_second,
    state_lea_register_first,
    state_lea_register_second,
    state_start_not_parser,
    state_not_register_first,
    state_not_register_second,
    state_not_register_third,
    state_start_ldr_parser,
    state_ldr_register_first,
    state_ldr_register_second,
    state_ldr_register_third,
    state_start_jmp_parser,
    state_get_jmp_token,
    state_start_jsr_parser,
    state_get_jsr_token,
    state_start_ret_parser,
    state_get_ret_token,
    state_start_ldi_parser,
    state_ldi_register_first,
    state_ldi_register_second,
    state_start_st_parser,
    state_st_register_first,
    state_st_register_second,
    state_start_sti_parser,
    state_sti_register_first,
    state_sti_register_second,
    state_start_str_parser,
    state_str_register_first,
    state_str_register_second,
    state_str_register_third,
    state_end_parser
} State;



typedef struct {
    char* origin;
    char* type;
    char* dest;
    char* sr1;
    char* sr2;
    char* sr3;
    char* imme;
    char* offset5;
    char* offset9;
    char* offset11;
    char* label;
    char* trap;
} vm_t;

struct node {
    vm_t* data;
    struct node* next;
};

typedef struct node node_t;

void append(node_t** node, vm_t** data) {

    node_t* new_node = malloc(sizeof *new_node);
    new_node->data = *data;
    new_node->next = NULL;

    if (*node == NULL) {
        *node = new_node;
        return;
    }

    node_t* tmp = *node;

    while (tmp->next != NULL) tmp = tmp->next;

    tmp->next = new_node;
}

void vm_print(vm_t* vm);

void node_print(node_t** node)
{
    node_t* tmp = *node;
    // vm_print(tmp->data);
    while(tmp != NULL)
    {
        vm_print(tmp->data);
        tmp = tmp->next;
    }
}


vm_t* vm_init(void) {

    vm_t* vm = malloc(sizeof *vm);
    vm->origin = NULL;
    vm->type = NULL;
    vm->dest = NULL;
    vm->sr1 = NULL;
    vm->sr2 = NULL;
    vm->sr3 = NULL;
    vm->imme = NULL;
    vm->offset5 = NULL;
    vm->offset9 = NULL;
    vm->offset11 = NULL;
    vm->label = NULL;
    vm->trap = NULL;
    return vm;
}

void vm_print(vm_t* vm) {

    printf("************** %s *************\n\n", vm->type);
    printf("vm->origin   : %s\n", vm->origin);
    printf("vm->type     : %s\n", vm->type);
    printf("vm->dest     : %s\n", vm->dest);
    printf("vm->sr1      : %s\n", vm->sr1);
    printf("vm->sr2      : %s\n", vm->sr2);
    printf("vm->sr3      : %s\n", vm->sr3);
    printf("vm->imme     : %s\n", vm->imme);
    printf("vm->offset5  : %s\n", vm->offset5);
    printf("vm->offset9  : %s\n", vm->offset9);
    printf("vm->offset11 : %s\n", vm->offset11);
    printf("vm->label    : %s\n", vm->label);
    printf("vm->trap     : %s\n\n", vm->trap);

}


void copy_string(char** in, int size) {
    *in = malloc(sizeof(char) * size);
    memset(*in, 0, size);
    sprintf(*in, "%.*s", (int)currentTokenLength, currentToken);
    currentTokenLength = 0;
}


int ltrim(const char* str)
{
    int len = 0;
    for (; *str == ' '; str++, len++) {}
    return len;
}

int parser(void) {

    node_t* list = NULL;
    vm_t* vm = vm_init();
    char buffer[300];

    while( fgets(buffer, 300, stdin) != NULL)
    {

        if (buffer[0] == ';')
        {
            printf("i am hear\n");
            memset(buffer, 0, 300);
            continue;
        }

        if (strlen(buffer) == 1 ) continue;

        State state = state_start_origin;
        int offset = ltrim(buffer);
        printf("buf: %s", buffer);

        while(state != state_next_line)
        {
            int c = buffer[offset++];
            bool action = false;
            if (c == '_') state = state_start_label;
            if (c == '$') state = state_start_trap;

            switch(state)
            {
                case state_start_trap:
                    state = state_next_line;
                case state_start_label:
                    state = state_next_line;
                case state_start_origin:
                    if (c == '\n' || c == '\r' || c == ' ') break;
                    if (c == '.')
                    {
                        state = state_start_origin_parser;
                    }
                    if (c == 'a' || c == 'A')
                    {
                        state = state_start_add_parser;
                        add_token(c);
                        break;
                    }
                    if (c == 'n'  || c == 'N')
                    {
                        state = state_start_not_parser;
                        add_token(c);
                        break;
                    }
                    if (c == 'b' || c == 'B')
                    {
                        state = state_start_br_parser;
                        add_token(c);
                        break;
                    }
                    if (c == 'l' || c == 'L')
                    {

                        state = state_start_lea_parser;
                        add_token(c);
                        break;
                    }
                    if (c == 'j' || c == 'J')
                    {
                        state = state_start_jmp_parser;
                        add_token(c);
                        break;
                    }
                    if (c == 'r' || c == 'R'){
                        state = state_start_ret_parser;
                        add_token(c);
                        break;
                    }
                    if (c == 's' || c == 'S')
                    {
                        state = state_start_st_parser;
                        add_token(c);
                        break;
                    }
                    break;

                case state_start_comment_line:
                    // printf("comment: %c", c);
                    if (c == '\r' || c == '\n')
                    {
                        state = state_next_line;
                    }
                    break;
                case state_start_origin_parser:
                    if (c == '\n' || c == '\r')
                    {
                        fprintf(stderr, "orig must memory address to start program\n");
                        return EXIT_FAILURE;
                    }
                    if (c == ' ')
                    {
                        state = state_get_memory_location;
                        copy_string(&vm->origin, 5);
                        break;
                    }
                    add_token(c);
                    break;
                case state_get_memory_location:
                    if (c == ';' || c == '\n' || c == '\r' || c == '\t')
                    {
                        state = state_start_comment_line;
                        copy_string(&vm->label, LBL_SIZE);
                        append(&list, &vm);
                        vm = vm_init();
                        break;
                    }
                    add_token(c);
                    break;

                /*******************************************/
                            // ADD //
                /**********************************************/

                case state_start_add_parser:
                    if (c == 'n' || c == 'N')
                    {
                        state = state_start_and_parser;
                        add_token(c);
                        break;
                    }
                    if (c == ' ')
                    {
                        state = state_add_register_first;
                        copy_string(&vm->type, 4);
                        break;
                    }
                    add_token(c);
                    break;
                case state_add_register_first:
                    if (c == '\n' || c == '\r')
                    {
                        state = state_error;
                        break;
                    }
                    if (c == ' ') break;
                    if (c == ',')
                    {
                        state = state_add_register_second;
                        copy_string(&vm->dest, 3);
                        break;
                    }
                    add_token(c);
                    break;
                case state_add_register_second:
                    if (c == '\n' || c == '\r')
                    {
                        state = state_error;
                        break;
                    }
                    if (c == ' ') break;
                    if (c == ',')
                    {
                        state = state_add_register_third;
                        copy_string(&vm->sr1, 3);
                        break;
                    }
                    add_token(c);
                    break;
                case state_add_register_third:
                    if (c == ',')
                    {
                        state = state_error;
                        break;
                    }
                    if (c == ' ') break;
                    if (c == ';' || c == '\n' || c == '\r' || c == '\t')
                    {
                        state = state_next_line;
                        copy_string(&vm->sr2, 3);
                        append(&list, &vm);
                        vm = vm_init();
                        break;
                    }
                    add_token(c);
                    break;

                /*******************************************
                            // AND //
                **********************************************/

                case state_start_and_parser:
                    if (c == ' ')
                    {
                        state = state_and_register_first;
                        copy_string(&vm->type, 4);
                        break;
                    }
                    add_token(c);
                    break;
                case state_and_register_first:
                    if (c == '\n' || c == '\r')
                    {
                        state = state_error;
                        break;
                    }
                    if (c == ' ') break;
                    if (c == ',')
                    {
                        state = state_and_register_second;
                        copy_string(&vm->dest, 3);
                        break;
                    }
                    add_token(c);
                    break;
                case state_and_register_second:

                    if (c == '\n' || c == '\r')
                    {
                        state = state_error;
                        fprintf(stderr, "warning: and must have 3 arguments\n");
                        break;
                    }
                    if (c == ' ') break;
                    if (c == ',')
                    {
                        state = state_and_register_third;
                        copy_string(&vm->sr1, 3);
                        break;
                    }
                    add_token(c);
                    break;
                case state_and_register_third:
                    if (c == ',')
                    {
                        state = state_error;
                        fprintf(stderr, "syntax error extra comma a the end of third register/imme5\n");
                        break;
                    }
                    if (c == ' ') break;
                    if (c == ';' || c == '\n' || c == '\r' || c == '\t')
                    {
                        state = state_next_line;
                        copy_string(&vm->sr2, 3);
                        append(&list, &vm);
                        vm = vm_init();
                        break;
                    }
                    add_token(c);
                    break;



                /*******************************************
                            // BR OP_CODE //
                **********************************************/

                case state_start_br_parser:
                    if (c == ' ')
                    {
                        state = state_get_br_token;
                        copy_string(&vm->type, 6);
                        break;
                    }
                    add_token(c);
                    break;
                case state_get_br_token:
                    if (c == ';' || c == '\n' || c == '\r' || c == '\t')
                    {
                        state = state_next_line;
                        copy_string(&vm->label, LBL_SIZE);
                        append(&list, &vm);
                        vm = vm_init();
                        break;
                    }
                    if (c == ' ') break;
                    add_token(c);
                    break;


                /*******************************************
                            // JUMP OP_CODE //
                **********************************************/

                case state_start_jmp_parser:
                    if (c == 's' || c == 'S')
                    {
                        state = state_start_jsr_parser;
                        add_token(c);
                        break;
                    }
                    if (c == ' ')
                    {
                        state = state_get_jmp_token;
                        copy_string(&vm->type, 4);
                        break;
                    }
                    add_token(c);
                    break;
                case state_get_jmp_token:
                    if (c == ';' || c == '\n' || c == '\r' || c == '\t')
                    {
                        state = state_next_line; //state_start_comment_line;
                        copy_string(&vm->label, LBL_SIZE);
                        append(&list, &vm);
                        vm = vm_init();
                        break;
                    }
                    if (c == ' ') break;
                    add_token(c);
                    break;

                /*******************************************
                            // LD OP_CODE //
                **********************************************/

                case state_start_ld_parser:
                    if (c == 'r' || c == 'R')
                    {
                        state = state_start_ldr_parser;
                        add_token(c);
                        break;
                    }
                    if (c == 'i' || c == 'I')
                    {
                        state = state_start_ldi_parser;
                        add_token(c);
                        break;
                    }
                    if (c == ' ')
                    {
                        state = state_ld_register_first;
                        copy_string(&vm->type, 3);
                        break;
                    }
                    add_token(c); break;
                case state_ld_register_first:
                    if (c == ' ') break;
                    if (c == ',')
                    {
                        state = state_ld_register_second;
                        copy_string(&vm->dest, 3); break;
                    }
                    add_token(c);
                    break;
                case state_ld_register_second:
                    if (c == ';' || c == '\n' || c == '\r' || c == '\t')
                    {
                        state = state_next_line;
                        copy_string(&vm->sr1, 20);
                        append(&list, &vm);
                        vm = vm_init();
                        break;
                    }
                    if (c == ' ') break;
                    add_token(c);
                    break;


                /*******************************************/
                            // NOT //
                /**********************************************/

                case state_start_not_parser:
                    if (c == ' ')
                    {
                        state = state_not_register_first;
                        copy_string(&vm->type, 4);
                        break;
                    }
                    add_token(c);
                    break;
                case state_not_register_first:
                    if (c == '\n' || c == '\r')
                    {
                        state = state_error;
                        break;
                    }
                    if (c == ' ') break;
                    if (c == ',')
                    {
                        state = state_not_register_second;
                        copy_string(&vm->dest, 3);
                        break;
                    }
                    add_token(c);
                    break;
                case state_not_register_second:
                    if (c == '\n' || c == '\r')
                    {
                        state = state_error;
                        break;
                    }
                    if (c == ' ') break;
                    if (c == ',')
                    {
                        state = state_not_register_third;
                        copy_string(&vm->sr1, 3);
                        break;
                    }
                    add_token(c);
                    break;
                case state_not_register_third:
                    if (c == ',')
                    {
                        state = state_error;
                        break;
                    }
                    if (c == ' ') break;
                    if (c == ';' || c == '\n' || c == '\r' || c == '\t')
                    {
                        state = state_next_line;
                        copy_string(&vm->sr2, 3);
                        append(&list, &vm);
                        vm = vm_init();
                        break;
                    }
                    add_token(c);
                    break;

                /*******************************************
                            // LEA OP_CODE //
                **********************************************/

                case state_start_lea_parser:
                    if (c == 'd' || c == 'D')
                    {
                        state = state_start_ld_parser;
                        add_token(c);
                        break;
                    }
                    if (c == '\t') break;
                    if (c == ' ')
                    {
                        state = state_lea_register_first;
                        copy_string(&vm->type, 4);
                        break;
                    }
                    add_token(c);
                    break;
                case state_lea_register_first:
                    if (c == ' ') break;
                    if (c == ',')
                    {
                        state = state_lea_register_second;
                        copy_string(&vm->dest, 3); break;
                    }
                    add_token(c);
                    break;
                case state_lea_register_second:
                    if (c == ' ') break;
                    if (c == ';' || c == '\n' || c == '\r' || c == '\t')
                    {
                        state = state_start_comment_line;
                        copy_string(&vm->sr1, 3);
                        append(&list, &vm);
                        vm = vm_init();
                        break;
                    }
                    add_token(c);
                    break;

                /*******************************************
                            // LDR OP_CODE //
                **********************************************/

                case state_start_ldr_parser:
                    if (c == ' ')
                    {
                        state = state_ldr_register_first;
                        copy_string(&vm->type, 4);
                        break;
                    }
                    add_token(c); break;
                case state_ldr_register_first:
                    if (c == ' ') break;
                    if (c == ',')
                    {
                        state = state_ldr_register_second;
                        copy_string(&vm->dest, 3); break;
                    }
                    add_token(c); break;
                case state_ldr_register_second:
                    if (c == ' ') break;
                    if (c  == ',')
                    {
                        state = state_ldr_register_third;
                        copy_string(&vm->sr1, 3);
                        break;
                    }
                    add_token(c); break;
                case state_ldr_register_third:
                    if (c == ' ') break;
                    if (c == ';' || c == '\n' || c == '\r' || c == '\t')
                    {
                        state = state_next_line;
                        copy_string(&vm->sr2, 20);
                        append(&list, &vm);
                        vm = vm_init();
                        break;
                    }
                    add_token(c); break;

                case state_start_jsr_parser:
                    if (c == ' ')
                    {
                        state = state_get_jsr_token;
                        copy_string(&vm->type, 4);
                        break;
                    }
                    add_token(c);
                    break;
                case state_get_jsr_token:
                    if (c == ';' || c == '\n' || c == '\r' || c == '\t')
                    {
                        state = state_next_line; //state_start_comment_line;
                        copy_string(&vm->label, LBL_SIZE);
                        append(&list, &vm);
                        vm = vm_init();
                        break;
                    }
                    if (c == ' ') break;
                    add_token(c);
                    break;


                /*******************************************
                            // LDI OP_CODE //
                **********************************************/

                case state_start_ldi_parser:
                    if (c == ' ')
                    {
                        state = state_ldi_register_first;
                        copy_string(&vm->type, 4);
                        break;
                    }
                    add_token(c); break;
                case state_ldi_register_first:
                    if (c == ' ') break;
                    if (c == ',')
                    {
                        state = state_ldi_register_second;
                        copy_string(&vm->dest, 3); break;
                    }
                    add_token(c);
                    break;
                case state_ldi_register_second:
                    if (c == ';' || c == '\n' || c == '\r' || c == '\t')
                    {
                        state = state_next_line;
                        copy_string(&vm->sr1, 20);
                        append(&list, &vm);
                        vm = vm_init();
                        break;
                    }
                    if (c == ' ') break;
                    add_token(c);
                    break;


                /*******************************************
                            // ST OP_CODE //
                **********************************************/

                case state_start_st_parser:
                    if (c == 'i' || c == 'I')
                    {
                        state = state_start_sti_parser;
                        add_token(c);
                        break;
                    }
                    if (c == 'r' || c == 'R')
                    {
                        state = state_start_str_parser;
                        add_token(c);
                        break;
                    }
                    if (c == ' ')
                    {
                        state = state_st_register_first;
                        copy_string(&vm->type, 4);
                        break;
                    }
                    add_token(c); break;
                case state_st_register_first:
                    if (c == ' ') break;
                    if (c == ',')
                    {
                        state = state_st_register_second;
                        copy_string(&vm->dest, 3); break;
                    }
                    add_token(c);
                    break;
                case state_st_register_second:
                    if (c == ';' || c == '\n' || c == '\r' || c == '\t')
                    {
                        state = state_next_line;
                        copy_string(&vm->sr1, 20);
                        append(&list, &vm);
                        vm = vm_init();
                        break;
                    }
                    if (c == ' ') break;
                    add_token(c);
                    break;

                /*******************************************
                            // STI OP_CODE //
                **********************************************/

                case state_start_sti_parser:
                    if (c == ' ')
                    {
                        state = state_sti_register_first;
                        copy_string(&vm->type, 4);
                        break;
                    }
                    add_token(c); break;
                case state_sti_register_first:
                    if (c == ' ') break;
                    if (c == ',')
                    {
                        state = state_sti_register_second;
                        copy_string(&vm->dest, 3); break;
                    }
                    add_token(c);
                    break;
                case state_sti_register_second:
                    if (c == ';' || c == '\n' || c == '\r' || c == '\t')
                    {
                        state = state_next_line;
                        copy_string(&vm->sr1, 20);
                        append(&list, &vm);
                        vm = vm_init();
                        break;
                    }
                    if (c == ' ') break;
                    add_token(c);
                    break;

                /*******************************************
                            // STR OP_CODE //
                **********************************************/

                case state_start_str_parser:
                    if (c == ' ')
                    {
                        state = state_str_register_first;
                        copy_string(&vm->type, 4);
                        break;
                    }
                    add_token(c); break;
                case state_str_register_first:
                    if (c == ' ') break;
                    if (c == ',')
                    {
                        state = state_str_register_second;
                        copy_string(&vm->dest, 3); break;
                    }
                    add_token(c); break;
                case state_str_register_second:
                    if (c == ' ') break;
                    if (c  == ',')
                    {
                        state = state_str_register_third;
                        copy_string(&vm->sr1, 3);
                        break;
                    }
                    add_token(c); break;
                case state_str_register_third:
                    if (c == ' ') break;
                    if (c == ';' || c == '\n' || c == '\r' || c == '\t')
                    {
                        state = state_next_line;
                        copy_string(&vm->sr2, 20);
                        append(&list, &vm);
                        vm = vm_init();
                        break;
                    }
                    add_token(c); break;

                case state_next_line:
                    break;

                case state_start_ret_parser:
                    if (c == ' ')
                    {
                        state = state_get_ret_token;
                        copy_string(&vm->type, 6);
                        break;
                    }
                    add_token(c);
                    break;
                case state_get_ret_token:
                    if (c == ';')
                    {
                        state = state_next_line;
                        copy_string(&vm->label, LBL_SIZE);
                        append(&list, &vm);
                        vm = vm_init();
                        break;
                    }
                    if (c == ' ') break;
                    add_token(c);
                    break;


                case state_error:
                    printf("error\n");
                    return -1;
            }
            if (state == state_next_line)
            {
                //printf("yes i am hear\n");
                memset(buffer, 0, 300);
                break;
            }
        }
    }
    char* ptr = NULL;

    printf("sizeof vm: %lu\n", sizeof(ptr));

    node_print(&list);
    //vm_print(vm);
}
int main(int argc, char** argv) {
    const char* code[4] = {".orig x3000  ; Starting address.",
                           "lea r0, #3  ; Load address of msg0 into r0",
                           "lea r0, #1	; Load address of msg1 into r0."};

    return parser();
}

/*

lea r0, msg0  ; Load address of msg0 into r0.
puts  ; Print contents of r0 (msg0).
lea r0, msg1	; Load address of msg1 into r0.
puts  ; Print contents of r0 (msg1 string).

lea r2, neg97   ; Load address of neg97 into r2.
ldr r2, r2, #0  ; Load contents of address in r2, into r2 (r2 now holds -97).
.end

*/
