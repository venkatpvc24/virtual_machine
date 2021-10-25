#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>


#include "parser.h"

/*

    caller is responsible for freeing memory;

*/

#define MAX_LBL_SIZE 30;

static char current_token[4096];
static size_t current_token_length;


static void add_token(char c) {
    // printf("token: %c", c);
    if (current_token_length < sizeof(current_token)) {
        current_token[current_token_length++] = c;
    }
}

char print_arr[100];

static void print_token(void) {
    sprintf(print_arr, "%.*s", (int)current_token_length, current_token);
    printf("print_arr: %s\n", print_arr);
    current_token_length = 0;
}

static void node_append(node_t** node, vm_t** data) {
    node_t* new_node = malloc(sizeof *new_node);

    if (new_node == NULL) {
        printf("error number: %d\n", errno);
        printf("error: %s", strerror(errno));
        exit(EXIT_FAILURE);
    }

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

static void vm_print(vm_t* vm);

static void node_print(node_t** node) {
    node_t* tmp = *node;
    // vm_print(tmp.data);
    while (tmp != NULL) {
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
    vm->label = NULL;
    vm->trap = NULL;
    vm->pesudo = NULL;
    vm->p_value = NULL;
    return vm;
}

void vm_print(vm_t* vm) {
    printf("************** %s *************\n\n", vm->type);
    printf("vm.origin   : %s\n", vm->origin);
    printf("vm.type     : %s\n", vm->type);
    printf("vm.dest     : %s\n", vm->dest);
    printf("vm.sr1      : %s\n", vm->sr1);
    printf("vm.sr2      : %s\n", vm->sr2);
    printf("vm.label    : %s\n", vm->label);
    printf("vm.pesudo   : %s\n", vm->pesudo);
    printf("vm.p_value  : %s\n", vm->p_value);
    printf("vm.trap     : %s\n\n", vm->trap);
}


static int vm_count_chars(char* str) {
    int len = 0;
    for (; *str == '\0'; str++) len++;
    return len;
}


static bool vm_is_end_of_line(char c)
{
    return c == '\t' || c == '\n' || c == '\r' || c == ';';
}

static void vm_go_to_next_line(char c, node_t** list, vm_t** vm, state* state) {
    if (vm_is_end_of_line(c)) {
        node_append(list, vm);
        *vm = vm_init();
        *state = state_next_line;
    }
}

static int vm_ltrim(const char* str) {
    int len = 0;
    while(isspace(*(str++))) len++;
    return len;
}

static void vm_to_lower(char* token)
{
    for(size_t i = 0; i < strlen(token); i++)
    {
        if (isupper(token[i])) token[i] |= 32;
    }
}

/*

copy the string either STR or CURRENT_TOKEN

*/

static void vm_copy_string(char** dest, int size, const char* str, bool lower) {

    *dest = malloc(sizeof(char) * size);
    if (*dest == NULL) {
        printf("error no: %d\n", errno);
        printf("error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    memset(*dest, 0, size);
    if (str == NULL) {
        sprintf(*dest, "%.*s", (int)current_token_length, current_token);
    } else {
        strcpy(*dest, str);
    }

    if (lower) vm_to_lower(*dest);
    current_token_length = 0;
}

int get_op(char* op);
static int vm_get_trap(char* op);

static int check_string(char* arr) {
    if (get_op(arr) >= 0) {
        return state_start_opcode_parser;
    }

    if (vm_get_trap(arr) >= 0) {
        return state_start_trap_parser;
    }
    return state_start_label_parser;
}



/*

    to-do:
        check is op_code has lower letters or upper
*/



int get_op(char* op_code) {
    const char* op_codes[17] = {"br",  "add", "ld",  "st",  "jsr", "and",
                                "ldr", "str", "rti", "not", "ldi", "sti",
                                "ret", "res", "lea", "trap", "pesudo"};

    if (isupper(op_code[0])) {
        for (size_t i = 0; i < strlen(op_code); i++) {
            if (op_code[i] != 'n' || op_code[i] != 'z' || op_code[i] != 'p')
                op_code[i] |= 32;
        }
    }
    if (op_code[0] == 'b' && op_code[1] == 'r') return 0;

    for (int i = 0; i < 17; i++) {
        if (strcmp(op_code, op_codes[i]) == 0) {
            return i;
        }
    }

    return -1;
}

static int vm_get_trap(char* trap) {
    const char* traps[5] = {"puts", "getc", "halt", "in", "out"};

    vm_to_lower(trap);

    for (int i = 0; i < 5; i++) {
        if (strcmp(trap, traps[i]) == 0) {
            return i;
        }
    }
    return -1;
}



/*
    pass linked list node to parser
    and parser will fill with tokens
*/

node_t* parser(const char* filename) {
    vm_t* vm = vm_init();
    node_t* list = NULL;
    char buffer[300];
    FILE* in = fopen(filename, "r");
    if (in == NULL) {
        printf("error no: %d\n", errno);
        printf("error: %s\n", strerror(errno));
    }
    while (fgets(buffer, 300, in)) {

        if (strlen(buffer) == 1) continue;

        state state = state_start_origin;  // change made
        int offset = vm_ltrim(buffer);

        if (buffer[offset] == ';') continue;

        while (state != state_next_line) {
            int c = buffer[offset++];
            char* str;

            switch (state) {
                case state_error:
                    printf("error\n");
                    break;
                case state_start_origin:
                    if (c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == ';') {
                        str = malloc(sizeof(char) * LBL_SIZE);
                        sprintf(str, "%.*s", (int)current_token_length,
                                current_token);  // change made
                        current_token_length = 0;
                        //printf("str: %s\n", str);
                        vm_to_lower(str);
                        if (str[0] == '.' && (strcmp(str, ".orig") == 0 || strcmp(str, ".end") == 0)) {
                            vm_copy_string(&vm->pesudo, current_token_length, str, true);
                            vm_copy_string(&vm->type, current_token_length, "pesudo", true);
                            node_append(&list, &vm);
                            vm = vm_init();
                            state = state_next_line;
                            break;
                        }
                        // printf("first str: %s\n", str);
                        state = check_string(str);
                        break;
                    }
                    add_token(c);
                    break;
                case state_start_label_parser:
                    if (str[0] != '.' && vm_is_end_of_line(c))
                    {
                        vm_copy_string(&vm->label, current_token_length, str, true);
                        vm_copy_string(&vm->type, current_token_length, "label", true);
                        vm_go_to_next_line(c, &list, &vm, &state);
                        break;
                    }
                    if (c == '.' || str[0] == '.') {
                        pesudo_t pesudo = start_pesudo_parser;
                        while (pesudo != next_line) {
                            switch (pesudo) {
                                case start_pesudo_parser: {
                                    if (c == ' ' && str[0] != '.') {
                                        vm_copy_string(&vm->pesudo, current_token_length, NULL, true);
                                        vm_copy_string(&vm->type, current_token_length, "pesudo", true);
                                        pesudo = start_p_value_parser;
                                        break;
                                    }
                                    if (str[0] == '.' && vm_is_end_of_line(c))
                                    {       vm_copy_string(&vm->p_value, current_token_length, NULL, false);
                                            vm_copy_string(&vm->pesudo, current_token_length, str, true);
                                            vm_copy_string(&vm->type, current_token_length, "pesudo", true);
                                            vm_go_to_next_line(c, &list, &vm, &state);
                                            break;
                                    }
                                    add_token(c);
                                    break;
                                }
                                case start_p_value_parser: {
                                    if (vm_is_end_of_line(c)) {
                                        vm_copy_string(&vm->p_value, current_token_length, NULL, true);
                                        vm_go_to_next_line(c, &list, &vm, &state);
                                        break;
                                    }

                                    add_token(c);
                                    break;
                                }
                            }
                            c = buffer[offset++];

                            if (state == state_next_line) {
                                break;
                            }
                        }
                    }
                    break;
                case state_start_trap_parser:
                    vm_copy_string(&vm->trap, current_token_length, str, true);
                    vm_copy_string(&vm->type, current_token_length, "trap", true);
                    vm_go_to_next_line(c, &list, &vm, &state);
                    break;
                case state_start_opcode_parser: {
                    opcodes_t op_state = get_op(str);
                    vm_copy_string(&vm->type, current_token_length, str, true);
                    while (state != state_next_line) {
                        switch (op_state) {
                            case state_add:
                            case state_and:
                            case state_ld:
                            case state_st:
                            case state_ldr:
                            case state_str:
                            case state_not:
                            case state_ldi:
                            case state_sti:
                            case state_lea:
                                if (c == '\n' || c == '\r') {
                                    state = state_error;
                                }
                                if (c == ',') {
                                    vm_copy_string(&vm->dest, current_token_length, NULL, true);
                                    op_state = state_second_register;
                                    break;
                                }
                                add_token(c);
                                break;
                            case state_br:
                                if (vm_is_end_of_line(c)) {
                                    vm_copy_string(&vm->label, current_token_length, NULL, true);
                                    vm_go_to_next_line(c, &list, &vm, &state);
                                    break;
                                }

                                if (isspace(c)) break;

                                add_token(c);
                                break;
                            case state_jsr:
                                if (vm_is_end_of_line(c)) {
                                    vm_copy_string(&vm->label, current_token_length, NULL, true);
                                    vm_go_to_next_line(c, &list, &vm, &state);
                                    break;
                                }
                                if (isspace(c)) break;
                                add_token(c);
                                break;
                            case state_ret:
                                vm_go_to_next_line(c, &list, &vm, &state);
                                current_token_length = 0;
                                break;
                            case state_second_register:
                                if (vm_is_end_of_line(c)) {
                                    if (str && (strcmp(str, "lea") == 0 ||
                                                strcmp(str, "ld") == 0 ||
                                                strcmp(str, "st") == 0 ||
                                                strcmp(str, "not") == 0 ||
                                                strcmp(str, "sti") == 0 ||
                                                strcmp(str, "ldi") == 0)) {
                                        vm_copy_string(&vm->sr1, current_token_length, NULL, true);
                                        vm_go_to_next_line(c, &list, &vm, &state);
                                    }
                                    break;
                                }
                                if (isspace(c)) break;
                                if (c == ',') {
                                    op_state = state_third_register;
                                    vm_copy_string(&vm->sr1, current_token_length, NULL, true);
                                    break;
                                }
                                add_token(c);
                                break;
                            case state_third_register:
                                if (c == ',') {
                                    state = state_error;
                                    break;
                                }
                                if (c == ' ') break;
                                if (vm_is_end_of_line(c)) {
                                    vm_copy_string(&vm->sr2, current_token_length, NULL, true);
                                    vm_go_to_next_line(c, &list, &vm, &state);
                                    break;
                                }
                                add_token(c);
                                break;
                            default:
                                state = state_next_line;
                                break;
                        }
                        c = buffer[offset++];
                    }
                    break;
                    case state_next_line:
                        break;
                }
            }
            if (state == state_next_line) {
                memset(buffer, 0, 300);
                break;
            }
        }
    }
    node_print(&list);
    // free(in);
    return list;
}
