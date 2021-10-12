#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>
#include <stdbool.h>




#define EQS(s1, s2) (strcmp(s1, s2) == 0)
#define TO_INT(s) ((s[1] - '0'))
#define CHECK_REG(num) (num >= 0 && num <= 3)



static char* arr[] = {"add", "br", "not", "str", "and", "lea", "ldr",
                  "br", "ret", "sti", "jsr", "ld", "ldi", "sti", "st", "brnzp"};


typedef enum
{
    ADD,
    BR,
    NOT
} op_t;


char* trim(char* s);

void READ_WORD(char* line, char word[5])
{
    int n = 0;
    while (*line != ' ') word[n++] = *(line++);
    word[n] = '\0';
}


int op_exists(char* s)
{

    for (int i = 0; i < 13; i++)
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

    for (int i = 0; i < strlen(s); i++) s[i] |= 32;

    for ( int i = 0; i < 16; i++)
    {
        if (strcmp(s, arr[i]) == 0)
        {
            return i;
        }
    }

    return -1;
}

/*
    grammer - ADD
    LC = label op reg reg reg oprnd comment | EOF
    label = [a-zA-Z0-9] | empty
    op = ADD | BR | NOT
    reg = R[0..3]
    oprnd = num | reg
    comment = [a-zA-Z09] | empty

*/

char* trim(char *s) {
    char *ptr;
    if (!s)
        return NULL;   // handle NULL string
    if (!*s)
        return s;      // handle empty string
    for (ptr = s + strlen(s) - 1; (ptr >= s) && isspace(*ptr); --ptr);
    ptr[1] = '\0';
    return s;
}


int is_op(char* token, size_t s)
{
    for (int i = 0; i < 3; i++) token[i] |= 32;
    return EQS(token, "add") || EQS(token, "not") || EQS(token, "br");
}

int is_reg(char* r)
{
    return CHECK_REG(TO_INT(r));
}

int is_num(char* n)
{
    return (n[0] == '#');
}

int is_oprnd(char* rnd)
{
    if (is_num(rnd)) return 1;
    else 0;
}

int op_four_register_processesor(char* i1, char* i2, char* i3)
{
    uint16_t bit;
    bit = (1 << 12);
    if (is_reg(i1) && is_reg(i2) && is_reg(i3))
    {
        bit |= (TO_INT(i1) << 9) + (TO_INT(i2) << 6);
        if (is_oprnd(i3) == 1)
        {
            bit |= (1 << 5) + 3;
        }
        else
        {
            bit |= (0 << 5) + 3;
        }
    }
}


int main()
{

    char op_code[6], i1[3], i2[3], i3[3];
    int op;

    FILE* in = stdin;
    char buf[100];
    while(fgets(buf, 100, in))
    {

        char* line = (char*)buf;
        char* c = line;


        READ_WORD(c, op_code);
        op = get_op(op_code);

        printf("op_code: %s, op: %d\n", op_code, op);


        switch(op)
        {
            case 0:
                sscanf(line+4, "%2s, %2s, %s", i1, i2, i3);
                printf("%2s, %2s, %s\n", i1, i2, i3);
                break;
            case 1:
                break;
            case 2:
                break;
            default:
            {
                //printf("done\n");
            }
        }

        memset(op_code, 0, 5);
        memset(i1, 0, 3);
        memset(i2, 0, 3);
        memset(i3, 0, 3);

    }

}
