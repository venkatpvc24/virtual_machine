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


static char currentToken[4096];
static size_t currentTokenLength;

static const char* op_codes[16] = {"br", "add", "ld", "st", "jsr", "and", "ldr",
                  "str", "rti", "not", "ldi", "sti", "ret", "res", "lea", "trap"};

static const char* traps[5] = {"puts", "getc", "halt", "in", "out"};

static void add_token(char c) {
    // printf("token: %c", c);
    if (currentTokenLength < sizeof(currentToken)) {
        currentToken[currentTokenLength++] = c;
    }
}

char print_arr[100];

/*
  static void print_token(void) {
      sprintf(print_arr, "%.*s", (int)currentTokenLength, currentToken);
      printf("print_arr: %s\n", print_arr);
      currentTokenLength = 0;
  }
*/

void append(node_t** node, vm_t** data) {

    node_t* new_node = malloc(sizeof *new_node);

    if (new_node == NULL)
    {
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

void vm_print(vm_t* vm);

void node_print(node_t** node)
{
    node_t* tmp = *node;
    // vm_print(tmp.data);
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


void copy_string(char** in, int size, const char* str) {

    *in = malloc(sizeof(char) * size);

    if (*in == NULL)
    {
        printf("error no: %d\n", errno);
        printf("error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    memset(*in, 0, size);

    if (str == NULL)
    {
        sprintf(*in, "%.*s", (int)currentTokenLength, currentToken);
    }
    else
    {
        strcpy(*in, str);
    }
    currentTokenLength = 0;
}

int get_op(char* op);
int get_trap(char* op);

int check_string(char* arr)
{
    //printf("trap: %s\n", arr);
    if (get_op(arr) >= 0)
    {
        //printf("op_true: %d\n", 1);
        return state_start_opcode_parser;
    }

    if(get_trap(arr) >= 0)
    {
        return state_start_trap_parser;
    }

    return state_start_label_parser;
}

int ltrim(const char* str)
{
    int len = 0;
    while(isspace(*(str++))) len++;
    return len;
}

/*

    to-do:
        check is op_code has lower letters or upper
*/


int count_chars(char* str)
{

    int len = 0;
    for (; *str == '\0'; str++) len++;

    return len;
}

int get_op(char* op_code)
{
    const char* op_codes[17] = {"br", "add", "ld", "st", "jsr", "and", "ldr",
                  "str", "rti", "not", "ldi", "sti", "ret", "res", "lea", "trap", "pesudo"};

    if (isupper(op_code[0]))
    {
        for(size_t i = 0; i < strlen(op_code); i++)
        {
            if (op_code[i] != 'n' || op_code[i] != 'z' || op_code[i] != 'p')
                op_code[i] |= 32;
        }
    }
    if (op_code[0] == 'b' && op_code[1] == 'r') return 0;

    for ( int i = 0; i < 17; i++)
    {
        //printf("op_code: %s\n", op_codes[i]);
        if (strcmp(op_code, op_codes[i]) == 0)
        {
            //printf("op_code: %s, op_codes: %s i: %d\n", op_code, op_codes[i], i);
            return i;
        }
    }

    return -1;
}



int get_trap(char* trap)
{
    int len = count_chars(trap);
    const char* traps[5] = {"puts", "getc", "halt", "in", "out"};
    if (isupper(trap[0]))
    {
        for(int i = 0; i < len; i++) trap[i] |= 32;
    }

    for(int i = 0; i < 5; i++)
    {
        if (strcmp(trap, traps[i]) == 0)
        {
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
    if (in == NULL)
    {
      printf("error no: %d\n", errno);
      printf("error: %s\n", strerror(errno));
    }
    while(fgets(buffer, 300, in))
    {
        //printf("buffer: %s\n", buffer);
       if (strlen(buffer) == 1) continue;

        State state = state_start_origin;
        int offset = ltrim(buffer);

        while(state != state_next_line)
        {
            int c = buffer[offset++];
            char* str;

            switch(state)
            {
                case state_error:
                  printf("error\n");
                  break;
                case state_start_origin:
                    if (c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == ';')
                    {

                        str = malloc(sizeof(char) * LBL_SIZE);
                        sprintf(str, "%.*s", (int)currentTokenLength, currentToken);
                        currentTokenLength = 0;
                        //printf("str: %s, c: %c\n", str, c);
                        if (str[0] == '.')
                        {
                            copy_string(&vm->pesudo, LBL_SIZE, str);
                            copy_string(&vm->type, LBL_SIZE, "pesudo");
                            append(&list, &vm);
                            vm = vm_init();
                            state = state_next_line;
                            break;
                        }
                        //printf("first str: %s\n", str);
                        state = check_string(str);
                        break;
                    }
                    add_token(c);
                    break;
                case state_start_label_parser:

                        copy_string(&vm->label, LBL_SIZE, str);
                        copy_string(&vm->type, LBL_SIZE, "label");
                        if (c == '\t' || c == '\n' || c == '\r' || c == ';')
                        {
                          printf("test: %s\n", str);
                          append(&list, &vm);
                          vm = vm_init();
                          state = state_next_line;
                          break;
                        }
                    if (c == '.')
                    {
                      pesudo_t pesudo = start_pesudo_parser;
                      bool is_string = false;
                      size_t str_len = 0;
                      while(pesudo != next_line)
                      {
                          switch(pesudo)
                          {
                              case start_pesudo_parser:
                              {
                                  if (c == ' ')
                                  {

                                      copy_string(&vm->pesudo, LBL_SIZE, NULL);
                                      copy_string(&vm->type, LBL_SIZE, "pesudo");
                                      if (strcmp(vm->pesudo, ".stringz") == 0 || strcmp(vm->pesudo, ".STRINGZ") == 0)
                                      {
                                          is_string = true;
                                      }
                                      pesudo = start_p_value_parser;

                                      break;
                                  }

                                  add_token(c);
                                  break;
                              }
                              case start_p_value_parser:
                              {
                                  if (is_string) str_len++;
                                  if (c == '\t' || c == '\n' || c == '\r' || c == ';')
                                  {
                                      //char* .stringz = malloc(sizeof(char) * str_len);
                                      copy_string(&vm->p_value, str_len, NULL);
                                      append(&list, &vm);
                                      vm = vm_init();
                                      state = state_next_line;
                                      break;
                                  }

                                  add_token(c);
                                  break;
                              }
                          }
                          c = buffer[offset++];

                            if (state == state_next_line)
                            {
                                break;
                            }

                        }
                    }
                    break;
                case state_start_trap_parser:
                        state = state_next_line;
                        printf("trap: %s\n", str);
                        copy_string(&vm->trap, LBL_SIZE, str);
                        copy_string(&vm->type, LBL_SIZE, "trap");
                        append(&list, &vm);
                        vm = vm_init();
                        break;
                case state_start_opcode_parser:
                {
                    op_codes_t op_state = get_op(str);
                    copy_string(&vm->type, LBL_SIZE, str);
                    while(state != state_next_line)
                    {
                        switch(op_state)
                        {
                            case state_add:
                                // printf("op_state str: %s\n", str);
                                if (c == '\n' || c == '\r')
                                {
                                    state = state_error;
                                }
                                if (c == ',')
                                {
                                    copy_string(&vm->dest, 3, NULL);
                                    op_state = state_second_register;
                                    break;
                                }
                                add_token(c);
                                break;
                            case state_and:
                                //printf("op_state str: %s\n", str);
                                if (c == '\n' || c == '\r')
                                {
                                    state = state_error;
                                }
                                if (c == ',')
                                {
                                    copy_string(&vm->dest, 3, NULL);
                                    op_state = state_second_register;
                                    break;
                                }
                                add_token(c);
                                break;
                            case state_br:
                                if (c == '\t' || c == '\n' || c == '\r' || c == ';')
                                {
                                    copy_string(&vm->label, LBL_SIZE, NULL);
                                    append(&list, &vm);
                                    vm = vm_init();
                                    state = state_next_line;
                                    break;
                                }
                                if (c == ' ')
                                {break;
                                }
                                add_token(c);
                                break;
                            case state_ld:
                                if (c == '\n' || c == '\r' || c == ';')
                                {
                                    state = state_error;
                                }
                                if (c == ',')
                                {
                                    copy_string(&vm->dest, 3, NULL);
                                    op_state = state_second_register;
                                    break;
                                }
                                add_token(c);
                                break;
                            case state_st:
                                if (c == '\n' || c == '\r' || c == ';')
                                {
                                    state = state_error;
                                }
                                if (c == ',')
                                {
                                    copy_string(&vm->dest, 3, NULL);
                                    op_state = state_second_register;
                                    break;
                                }
                                add_token(c);
                                break;
                            case state_jsr:
                                if (c == '\t' || c == '\n' || c == '\r' || c == ';')
                                {
                                    copy_string(&vm->label, LBL_SIZE, NULL);
                                    append(&list, &vm);
                                    vm = vm_init();
                                    state = state_next_line;
                                    break;
                                }
                                if (c == ' ') break;
                                add_token(c);
                                break;
                            case state_ldr:
                                if (c == '\n' || c == '\r')
                                {
                                    state = state_error;
                                }
                                if (c == ',')
                                {
                                    copy_string(&vm->dest, 3, NULL);
                                    op_state = state_second_register;
                                    break;
                                }
                                add_token(c);
                                break;
                            case state_str:
                                if (c == '\n' || c == '\r')
                                {
                                    state = state_error;
                                }
                                if (c == ',')
                                {
                                    copy_string(&vm->dest, 3, NULL);
                                    op_state = state_second_register;
                                    break;
                                }
                                add_token(c);
                                break;
                            case state_not:
                                if (c == '\n' || c == '\r')
                                {
                                    state = state_error;
                                }
                                if (c == ',')
                                {
                                    copy_string(&vm->dest, 3, NULL);
                                    op_state = state_second_register;
                                    break;
                                }
                                add_token(c);
                                break;
                            case state_ldi:
                                if (c == '\n' || c == '\r')
                                {
                                    state = state_error;
                                }
                                if (c == ',')
                                {
                                    copy_string(&vm->dest, 3, NULL);
                                    op_state = state_second_register;
                                    break;
                                }
                                add_token(c);
                                break;
                            case state_sti:
                                if (c == '\n' || c == '\r')
                                {
                                    state = state_error;
                                }
                                if (c == ',')
                                {
                                    copy_string(&vm->dest, 3, NULL);
                                    op_state = state_second_register;
                                    break;
                                }
                                add_token(c);
                                break;
                            case state_ret:
                                    state = state_next_line;
                                    currentTokenLength = 0;
                                    append(&list, &vm);
                                    vm = vm_init();
                                    break;
                            case state_lea:
                                if (c == '\n' || c == '\r')
                                {
                                    state = state_error;
                                }
                                if (c == ',')
                                {
                                    copy_string(&vm->dest, 3, NULL);
                                    op_state = state_second_register;
                                    break;
                                }
                                add_token(c);
                                break;
                            case state_second_register:
                                if (c == ';' || c == '\n' || c == '\r' || c == '\t')
                                {
                                    state = state_next_line;
                                    if (str && (strcmp(str, "lea") == 0 ||
                                                strcmp(str, "ld") == 0  ||
                                                strcmp(str, "st") == 0  ||
                                                strcmp(str, "not") == 0 ||
                                                strcmp(str, "sti") == 0 ||
                                                strcmp(str, "ldi") == 0))
                                    {
                                        copy_string(&vm->sr1, LBL_SIZE, NULL);
                                        append(&list, &vm);
                                        vm = vm_init();
                                    }
                                    break;
                                }
                                if (c == ' ') break;
                                if (c == ',')
                                {
                                    op_state = state_third_register;
                                    copy_string(&vm->sr1, 3, NULL);
                                    break;
                                }
                                add_token(c);
                                break;
                            case state_third_register:
                                if (c == ',')
                                {
                                    state = state_error;
                                    break;
                                }
                                if (c == ' ') break;
                                if (c == ';' || c == '\n' || c == '\r' || c == '\t')
                                {
                                    state = state_next_line;
                                    copy_string(&vm->sr2, LBL_SIZE, NULL);
                                    append(&list, &vm);
                                    vm = vm_init();
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
            if (state == state_next_line)
            {
                memset(buffer, 0, 300);
                break;
            }
        }
    }
    //node_print(&list);
    //free(in);
    return list;
}
