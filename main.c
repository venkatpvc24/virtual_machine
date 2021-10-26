/*#include <stdio.h>
#include <stdlib.h>

#include "parser.h"
#include "assembler.h"

int main(int argc, char** argv)
{
  uint16_t output[UINT16_MAX];
  uint16_t size = 0;
  uint16_t start_address = 0;
  node_t* node = parser(argv[1]);
  assembler(&node, output, &size, &start_address);
  execute_instructions(output, size, start_address);
}*/


#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>


// possiblities
// may start with .
// may start with ' '
// may start with ;
// may start with "
// may start with '
// may start with alpha
// may start with x
// may start with #


static char line[500];


typedef enum
{
    TK_START,
    TK_COMMA,
    TK_SPACE,
    TK_SEMICOLON,
    TK_QUOTES,
    TK_ALPHA,
    TK_NUMBER
} state_t;

typedef struct
{
    char* current;
    int count;
} token_t;


token_t token;
char *c;

state_t next_token(char *c)
{

    while(isspace(*c)) ++c;
    printf("next: %c\n", *c);
    //state_t state = TK_START;
    switch(*c)
    {
        case ',':
            c++;
            printf("comman: %c\n", *c);
            token.current = c;
            return TK_COMMA;
        case ';':
            return TK_SEMICOLON;
        case '\'':
        case '"':
            c++;
            token.current = c;
            return TK_QUOTES;
        case '#':
        case 'x':
            c++;
            token.current = c;
            return TK_NUMBER;
        default:
        {
            token.current = c;
            printf("inside alpah\n");
            return TK_ALPHA;
        }


    }
}

int main()
{

    char *sentance = "    add r0, r1, #3";
    strcpy(line, sentance);

    c = line;

    while(*c != '\0')
    {
        state_t state = next_token(c);
        switch(state)
        {
            case TK_ALPHA:
                while(*c != ' ')
                {
                    printf("inwhile: %c\n", *c);
                    c++;
                    token.count++;
                }
                printf("%.*s\n", token.count, token.current);
                token.count = 0;
                //c--;
                break;
            case TK_COMMA:
                printf("its comma\n");
                if (isspace(*c)) break;
                break;
            case TK_NUMBER:
                if (isspace(*c)) break;
                printf("its number\n");
                break;
            case TK_SEMICOLON:
                exit(EXIT_SUCCESS);
            defualt:
                printf("default done\n");
        }
    }
}
