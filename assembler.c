#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>


#include "parser.h"
#include "assembler.h"



int parse_number(const char* reg)
{
    int number;
    char* end;
    if (reg[0] == 'x') number = strtoul(reg + 1, &end, 16);
    if (reg[0] == '#') number = strtoul(reg+1, &end, 10);
    if (reg[0] == 'r') number = strtoul(reg+1, &end, 10);
    return number;
}

int op_exists(char* s)
{

    for (int i = 0; i < 15; i++)
    {
        if (strcmp(s, arr[i]))
        {
            return true;
        }
    }

    return false;
}


/*int get_op(char* s)
{

    for (size_t i = 0; i < strlen(s); i++) s[i] |= 32;

    if (strcmp(s, "br") == 0|| strcmp(s, "brn") == 0 || strcmp(s, "brnz") == 0 || strcmp(s, "brnzp") == 0) return 0;

    for ( int i = 0; i < 16; i++)
    {
        if (strcmp(s, arr[i]) == 0)
        {
            return i;
        }
    }

    return -1;
}*/




int parse_register(const char* reg)
{
    int register_number = TO_INT(reg);

    if (!(register_number >= 0 && register_number <= 7)) return INVALID_REG;

    return VM_SUCESS;
}


imme_reg_t* is_imme_r_reg(const char* imme)
{

    imme_reg_t* type = malloc(sizeof *type);
    type->imme = false;
    type->reg = false;

    if (imme[0] == 'R' || imme[0] == 'r') type->reg = true;

    if (imme[0] == '#')
    {
        type->imme = true;
        type->value = parse_number(imme);
    }

    return type;

}


/*
    function to check is immediate number with in the offset limit
    return out of range error if not, otherwise valid_number.
*/

int valid_number(const char* num, int offset)
{

    if (num[0] != '#') return INVALID_NUM_SYNTAX;

    int val = parse_number(num);

    switch(offset)
    {
    	case 5:
    	{
    		if (val >= -16 && val <= 15) return VM_VALID_NUMBER;
    		return OUT_OF_RANGE_IMME5;
    	}
    	case 9:
    	{
    		if (val >= -256 && val <= 255) return VM_VALID_NUMBER;
    		return OUT_OF_RANGE_IMME9;
    	}
    }

    return VM_SUCESS;
}


uint16_t sign_extend(uint16_t x, int bit_count) {
    if ((x >> (bit_count - 1)) & 1) {
        x |= (0xFFFF << bit_count);
    }
    return x;
}

int PC_Offset9(int op, const char* op_code, const char* r1, const char* offset, int line, uint16_t* bit)
{
    *bit = 0;

    if (parse_register(r1) == INVALID_REG)
    {
        fprintf(stderr, "warning %d: %s expression has invalid register %s\n",
                line, op_code, r1);
        return EXIT_FAILURE;

    }

    int address = parse_number(offset);
    //printf("address: %d\n", address);
    *bit = (op << 12) + (TO_INT(r1) << 9) + (address & 0x1FF);
    return 0;
}

int PC_Offset5(int op, const char* op_code, const char* dreg, const char* reg, const char* offset5, int line_count, uint16_t* bit)
{

    *bit = 0;
    if (parse_register(dreg) == INVALID_REG || parse_register(reg) == INVALID_REG)
    {
        fprintf(stderr, "warning %d: %s expression has invalid register %s or %s\n", line_count, op_code, dreg, reg);
        return EXIT_FAILURE;
    }

    int address = parse_number(offset5);

    *bit |= (op << 12) + (TO_INT(dreg) << 9) + (TO_INT(reg) << 6) + (address & 0x1F);

    return 0;
}

int dr_sr_imme5(int op, const char* op_code, const char* dreg, const char* reg, const char* imme5, int line_count, uint16_t* bit)
{

    *bit = 0;
    if (parse_register(dreg) == INVALID_REG || parse_register(reg) == INVALID_REG)
    {
        fprintf(stderr, "warning %d: %s expression has invalid register %s or %s\n", line_count, op_code, dreg, reg);
        return EXIT_FAILURE;
    }

    imme_reg_t* type = is_imme_r_reg(imme5);

    if (type->imme == true)
    {
        if (valid_number(imme5, 5) == OUT_OF_RANGE_IMME5)
        {
            fprintf(stderr, "warning %d: value must be between -16 and 15, but value is %d\n", line_count, type->value);
            return EXIT_FAILURE;
        }
        *bit |= (op << 12) + (TO_INT(dreg) << 9) + (TO_INT(reg) << 6) + (1 << 5) + (type->value & 0x1F);
    }

    if (type->reg == true)
    {
        if (parse_register(imme5) == INVALID_REG)
        {
            fprintf(stderr, "warning %d: ADD expression has invalid register %s\n", line_count, imme5);
            return EXIT_FAILURE;
        }

        if (valid_number(imme5, 9) == OUT_OF_RANGE_IMME9)
        {
            fprintf(stderr, "warning %d: value must be between -256 and 255, but value is %d\n", line_count, type->value);
            return EXIT_FAILURE;
        }

        *bit = (op << 12) + (TO_INT(dreg) << 9) + (TO_INT(reg) << 6) + (0 << 5) + (TO_INT(imme5));
    }

    return 0;
}



typedef struct
{
    char* token[5];
    int count;
} tokens_t;

void print_tokens(tokens_t* tokens)
{
    for (int i = 0; i < tokens->count; i++)
    {
        fprintf(stderr, "token: %s, len: %lu\n", tokens->token[i], strlen(tokens->token[i]));
    }
}



int is_end_of_str(char c)
{
    return c == '\0' || c == ' ' || c == ',' || c == ';' || c == ':' || c == '\n';
}

int is_comment(char c)
{
    return c == ';';
}

//int is_valid_operand()


uint16_t find_address(char* label, node_t* node)
{
    uint16_t address = 0;
    node_t* list = node;

    for (size_t i = 0; i < strlen(label); i++) label[i] |= 32;
    bool action = false;
    while(list != NULL)
    {
        if (list->data->label != NULL && strcmp(list->data->type, "label") == 0)
        {

          //printf("type: %s, data->label: %s, address: %d\n", list->data->type, list->data->label, address);
          if (strcmp(label, list->data->label) == 0) break;
        }

        if(strcmp(list->data->type, "label") != 0 && strcmp(list->data->type, "pesudo") != 0)
        {
          address++;
        }

      list = list->next;

    }

    return address;
}

void assembler(node_t** node, uint16_t* output, uint16_t* size)
{


    node_t* tmp = *node; // parsed data
    uint16_t line = 1, index = 0;
    uint16_t bit = 0;
    uint16_t bin_data[UINT16_MAX]; // assembled data
    int len = 0;


    while(tmp != NULL)
      {
          vm_t* vm = tmp->data;

          if (strcmp(vm->type, "label") == 0)
          {
                tmp = tmp->next;
                continue;
          }

          // char* op_code = (vm->type == NULL) ? vm->trap : vm->type;
          uint16_t op = get_op(vm->type);

          //printf("op: %d, type: %s\n", op, vm->type);


          switch(op)
          {
              case 0: // BR
                  {
                      bit = 0;

                      char c1 = vm->type[2];
                      char c2 = vm->type[3];
                      char c3 = vm->type[4];

                      bit |= (c1 == 'n' ? (1 << 11) : (0 << 11));
                      bit |= (c1 == 'z'  || c2 == 'z' ? (1 << 10) : (0 << 10));
                      bit |= (c1 == 'p' || c2 == 'p' || c3 == 'p' ? (1 << 9) : (0 << 9));

                      int address = find_address(vm->label, *node);


                      //printf("label: %s, address: %d, line: %d, current: %d\n", vm->label, address, line, address - line);

                      bit |= ((address - line) & 0x1FF);
                      output[index++] = bit;
                      //printf("address: x%04x\n", bin_data[index] + 3017);
                      line++;
                  }
                  break;
              case state_and:
              case state_add: // ADD
                  {

                      dr_sr_imme5(op, vm->type,
                                      vm->dest,
                                      vm->sr1,
                                      vm->sr2, line, &bit);
                      line++;
                      output[index++] = bit;

                  }
                  break;
              case state_jsr: // JSR
                  {
                      bit = 0;
                      int address = find_address(vm->label, *node);

                      output[index++] = bit;
                      line++;
                      break;
                  }
              case state_str:
              case state_ldr: // LDR
                  {

                      PC_Offset5(op, vm->type,
                                      vm->dest,
                                      vm->sr1,
                                      vm->sr2, line, &bit);
                      line++;
                      output[index++] = bit;

                      break;
                  }
              case state_not: // NOT
                  {
                      bit = 0;

                      bit |= (9 << 12) + (TO_INT(vm->dest) << 9) + (TO_INT(vm->sr1) << 6) + 0x3F;
                      line++;
                      output[index++] = bit;
                      break;
                  }
              case state_ld:
              case state_lea:
              case state_st:
              case state_ldi: // LDI
              case state_sti: // STI
                  {

                      PC_Offset9(op, vm->type,
                                     vm->dest,
                                     vm->sr1, line, &bit);

                      output[index++] = bit;
                      line++;
                      break;
                  }
              case state_ret: // RET
                  {
                      bit = 0;
                      bit |= (12 << 12) + 0 + (0xE << 5) + 0;
                      output[index++] = bit;
                      line++;
                      break;
                  }
              case state_trap: // RES
                  {
                      bit = 0xF000;

                      if (strcmp(vm->trap, "getc") == 0) bit |= (0x20 & 0xFF);
                      if (strcmp(vm->trap, "out") == 0) bit |= (0x21 & 0xFF);
                      if (strcmp(vm->trap, "puts") == 0) bit |= (0x22 & 0xFF);
                      if (strcmp(vm->trap, "in") == 0) bit |= (0x23 & 0xFF);
                      if (strcmp(vm->trap, "halt") == 0) bit |= (0x25 & 0xFF);

                      output[index++] = bit;
                      line++;
                      break;
                  }
              case state_pesudo:
                {
                  if (strcmp(vm->pesudo, ".orig") != 0 || strcmp(vm->pesudo, ".end") != 0)
                  {
                    if (strcmp(vm->pesudo, ".fill") == 0 || strcmp(vm->pesudo, ".FILL") == 0)
                    {
                      //printf("number: %d", parse_number(vm->p_value));
                      output[index++] = parse_number(vm->p_value);
                      line++;
                    }

                    if (strcmp(vm->pesudo, ".stringz") == 0 || strcmp(vm->pesudo, ".STRINGZ") == 0)
                    {
                      //printf("number: %d", parse_number(vm->p_value));
                      for(size_t s = 0; s < strlen(vm->p_value); s++)
                      {
                        if (vm->p_value[s] != '"' ) output[index++] = vm->p_value[s];
                        line++;
                      }
                      output[index++] = '\0';
                    }

                  }


                  break;
                }
              default:
              {
                  //printf("done\n");
              }
          }
          tmp = tmp->next;
      }
      *size = index;
   //printf("index: %d", index);
   //for ( int i = 0; i < index; i++ ) printf("ip: x%04x, code: 0x%04x\n", i + 12288, bin_data[i]);

}
