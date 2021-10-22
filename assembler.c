#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>


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



int PC;

static const char* arr[16] = {"br", "add", "ld", "st", "jsr", "and", "ldr",
                  "str", "rti", "not", "ldi", "sti", "ret", "res", "lea", "trap"};


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




typedef struct
{
    char label[100];
    int address;
} label_t;



int parse_number(const char* reg)
{
    int number;
    sscanf(reg+1, "%d", &number);
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


int get_op(char* s)
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
}




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
    printf("address: %d\n", address);
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
        *bit |= (op << 12) + (TO_INT(dreg) << 9) + (TO_INT(reg) << 6) + (1 << 5) + type->value;
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

int tokenizer(const char* code, tokens_t* tokens)
{
    //const char* code = "add r1, r2, r2 ; add register";

    int i = 0, count = 0;

    while(code[i] != '\0')
    {
        char token[5];
        int n = 0;
        memset(token, 0, 5);

        if (is_comment(code[i]) == 1) break;

        while(is_end_of_str(code[i]) != 1)
        {
            if (is_comment(code[i]) == 1) break;
            token[n++] = code[i++];
        }

        token[n] = '\0';
        if (strcmp(token, "") != 0)
        {
            tokens->token[count] = malloc(sizeof(char) * n);
            strcpy(tokens->token[count++], token);
        }
        i++;
    }

    tokens->count = count;

}


int assembler(FILE* in)
{
    char buf[250];

    uint16_t line = 0;
    uint16_t bit = 0;
    uint16_t bin_data[UINT16_MAX];

    label_t labels[100];
    int len = 0;

    while(fgets(buf, 250, in))
    {

        if (strlen(buf) == 1) continue;

        char* tokens[];

        tokenizer(buf, tokens);

        int op = get_op(tokens->token[0]);

        if (op == -1 && strcmp(tokens->token[0], ".orig") != 0)
        {

            strcpy(labels[len].label, tokens->token[0]);
            printf("len: %d, label: %s\n", len, labels[len].label);
            labels[len].address = line; len++;
            continue;
        }


        switch(op)
        {
            case 0:
                {
                    bit = 0;
                    char brnzp[3];
                    strcpy(brnzp, tokens->token[0] + 2);

                    bit |= (brnzp[0] == 'n' ? (1 << 11) : (0 << 11));
                    bit |= (brnzp[1] == 'z' ? (1 << 10) : (0 << 10));
                    bit |= (brnzp[2] == 'p' ? (1 << 9) : (0 << 9));

                    int address; // = labels[lbl_len].address - line

                    for (int i = 0; i < len; i++)
                    {
                        if (strcmp(tokens->token[1], labels[i].label) == 0)
                        {
                            address = labels[i].address - line;
                            break;
                        }
                    }

                    bit |= (address & 0x1FF);
                    bin_data[line] = bit;
                    line++;

                }
                break;
            case 1: // ADD
                {
                    if (tokens->count < 3)
                    {
                        fprintf(stderr, "warning %d: add expression must expect 3 arguments but found %d\n", line, tokens->count - 1);
                        return EXIT_FAILURE;
                    }

                    dr_sr_imme5(op, tokens->token[0],
                                    tokens->token[1],
                                    tokens->token[2],
                                    tokens->token[3], line, &bit);

                    bin_data[line] = bit;

                }
                break;
            case 2: // LD
                {

                    if (tokens->count < 2)
                    {
                        fprintf(stderr, "warning %d: LD expression must expect 2 arguments but found %d\n", line, tokens->count-1);
                        return EXIT_FAILURE;
                    }

                    PC_Offset9(op, tokens->token[0],
                                   tokens->token[1],
                                   tokens->token[2], line, &bit);

                    bin_data[line] = bit;
                }

                break;
            case 3: // ST
                {

                    if (tokens->count < 2)
                    {
                        fprintf(stderr, "warning %d: ST expression must expect 2 arguments but found %d\n", line, tokens->count);
                        return EXIT_FAILURE;
                    }

                    PC_Offset9(op, tokens->token[0],
                                   tokens->token[1],
                                   tokens->token[2], line, &bit);

                    bin_data[line] = bit;

                }
                break;
            case 4: // JSR
                {
                    bit = 0;

                    if (tokens->count < 1)
                    {
                        fprintf(stderr, "warning %d: JSR expression must expect 1 arguments but found %d\n",
                                line, tokens->count);
                        return EXIT_FAILURE;
                    }


                    /*int address = labels[lbl_len].address - line;
                    printf("addres: %d\n", address);

                    bit = (4 << 12) + (1 << 11) + (address & 0x7FF);

                    bin_data[line] = bit;

                    break;*/
                }
            case 5: // AND
                {
                    if (tokens->count < 4)
                    {
                        fprintf(stderr, "warning %d: AND expression must expect 3 arguments but found %d\n",
                                line, tokens->count);
                        return EXIT_FAILURE;
                    }


                    dr_sr_imme5(op, tokens->token[0],
                                    tokens->token[1],
                                    tokens->token[2],
                                    tokens->token[3], line, &bit);

                    bin_data[line] = bit;
                    break;
                }
            case 6: // LDR
                {
                    if (tokens->count < 3)
                    {
                        fprintf(stderr, "warning %d: LDR expression must expect 3 arguments but found %d\n",
                              line, tokens->count);
                        return EXIT_FAILURE;
                    }

                    PC_Offset5(op, tokens->token[0],
                                   tokens->token[1],
                                   tokens->token[2],
                                   tokens->token[3], line, &bit);

                    bin_data[line] = bit;

                    break;
                }
            case 7: // STR
                {

                    if (tokens->count < 4)
                    {
                        fprintf(stderr, "warning %d: STR expression must expect 3 arguments but found %d\n", line, tokens->count);
                        return EXIT_FAILURE;
                    }



                    PC_Offset5(op, tokens->token[0],
                    		   tokens->token[1],
                    		   tokens->token[2],
                    		   tokens->token[3], line, &bit);

                    bin_data[line] = bit;

                    break;

                }
            case 9: // NOT
                {
                    bit = 0;

                    if (tokens->count < 3)
                    {
                        fprintf(stderr, "warning %d: NOT expression must expect 2 arguments but found %d\n", line, tokens->count);
                        return EXIT_FAILURE;
                    }

                    bit |= (9 << 12) + (TO_INT(tokens->token[1]) << 9) + (TO_INT(tokens->token[2]) << 6) + 0x3F;

                    bin_data[line] = bit;
                    break;
                }
            case 10: // LDI
                {

                    if (tokens->count < 2)
                    {
                        fprintf(stderr, "warning %d: LDI expression must expect 2 arguments but found %d\n", line, tokens->count);
                        return EXIT_FAILURE;
                    }

                    PC_Offset9(op, tokens->token[0],
                    		   tokens->token[1],
                    		   tokens->token[2], line, &bit);

                    bin_data[line] = bit;
                    break;
                }
            case 11: // STI
                {

                    if (tokens->count < 2)
                    {
                        fprintf(stderr, "warning %d: STI expression must expect 2 arguments but found %d\n", line, tokens->count);
                        return EXIT_FAILURE;
                    }

                    PC_Offset9(op, tokens->token[0],
                    		   tokens->token[1],
                    		   tokens->token[2], line, &bit);

                    bin_data[line] = bit;
                    break;
                }
            case 12: // RET
                {
                    bit = 0;

                    if (tokens->count < 2)
                    {
                        fprintf(stderr, "warning %d: STI expression must expect 2 arguments but found %d\n", line, tokens->count);
                        return EXIT_FAILURE;
                    }

                    bit |= (12 << 12) + 0 + (0xE << 5) + 0;
                    bin_data[line] = bit;

                    break;
                }
            case 13: // RES
                {
                    break;
                }
            case 14: // LEA
                {
                    print_tokens(tokens);

                    if (tokens->count < 3)
                    {
                        fprintf(stderr, "warning %d: LEA expression must expect 2 arguments but found %d\n", line, tokens->count);
                        return EXIT_FAILURE;
                    }

                    PC_Offset9(op, tokens->token[0],
                    		   tokens->token[1],
                    		   tokens->token[2], line, &bit);

                    bin_data[line] = bit;
                    break;
                }
            default:
            {
                //printf("done\n");
            }
        }

        line++;
        memset(buf, 0, 5);

        free(tokens);

    }


   for ( int i = 0; i < 16; i++ ) printf("code: 0x%04x\n", bin_data[i]);



}

int main(int argc, char** argv)
{
    FILE* in = stdin;
    assembler(in);
}
