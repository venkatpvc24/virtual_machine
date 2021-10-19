#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>


static char currentToken[4096];
static size_t currentTokenLength;

static void add_token ( char c ) {
    // printf("token: %c", c);
    if (currentTokenLength < sizeof(currentToken)) {
        currentToken[currentTokenLength++] = c;
    }
}

static void print_token( ) {
    printf("Token: >>>%.*s<<<\n", (int)currentTokenLength, currentToken);
    currentTokenLength = 0;
}

typedef enum
{
    state_error,
    state_start_origin,
    state_origin,
    find_state_number,
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
    state_get_br_token
} State;

State state = state_start_origin;

int parser()
{
    while(state != state_error)
    {
        int c = fgetc(stdin);
        if (c == '_')
        {
            state = state_start_label;
        }
        switch(state)
        {
            case state_start_label:
                if (c == '\n' || c == '\r')
                {
                    state = state_next_line;
                }
                if (c == ' ')
                {
                    print_token();
                    break;
                }
                if (c == ';')
                {
                    state = state_start_comment_line;
                    break;
                }
                add_token(c);
                break;
            case state_start_origin:
                if (c == '\n' || c == '\r' || c == ' ') break;
                if (c == '.')
                    state = state_origin;
                else
                    state = state_error;
                break;
            case state_origin:
                if (c  == 'o') state = state_start_origin_parser;
            case state_start_origin_parser:
                if (c == ' ')
                {
                    state = state_end_origin_parser;
                    break;
                }
                add_token(c);
                break;
            case state_end_origin_parser:
                print_token();
                if (c == 'x') state = state_start_number_parser;
                if (c == '\r' || c == '\n' || c == '\0') state = state_end_line_parser;
                break;
            case state_start_number_parser:
                if (c == ' ' || c == '\r' || c == '\n' || c == '\0')
                {
                    state = state_end_number_parser;
                    break;
                }
                //printf("c: %c", c);
                add_token(c);
                break;
            case state_end_number_parser:
                print_token();
                if (c == ';') state = state_start_comment_line;
                break;



            /*******************************************
                        // COMMENT //
            **********************************************/

            case state_start_comment_line:
                if (c == '\n' || c == '\r') state = state_end_comment_line;
                //printf("c: %c\n", c);
                break;
            case state_next_line:
            case state_end_comment_line:
                if (c == 'a')
                {
                    state = state_start_add_parser;
                    add_token(c);
                    break;
                }
                if (c == 'b')
                {
                    state = state_start_br_parser;
                    add_token(c);
                    break;
                }
                if (c == '.')
                {
                    state = state_error;
                }
                break;


            /*******************************************/
                        // AND //
            /**********************************************/

            case state_start_add_parser:
                if (c == 'n')
                {
                    state = state_start_and_parser;
                    add_token(c);
                    break;
                }
                if (c == ' ')
                {
                    state = state_add_register_first;
                    print_token();
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
                    print_token();
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
                    print_token();
                    break;
                }
                add_token(c);
                break;
            case state_add_register_third:
                if (c == '\n' || c == '\r')
                {
                    state = state_next_line;
                    break;
                }
                if (c == ',')
                {
                    state = state_error;
                    break;
                }
                if (c == ' ') break;
                if (c == ';')
                {
                    state = state_start_comment_line;
                    print_token();
                    break;
                }
                add_token(c);
                break;
            case state_add_register_end:
                break;



            /*******************************************
                        // AND //
            **********************************************/

            case state_start_and_parser:
                if (c == ' ')
                {
                    state = state_and_register_first;
                    print_token();
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
                    print_token();
                    break;
                }
                add_token(c);
                break;
            case state_and_register_second:

                if (c == '\n' || c == '\r')
                {
                    state = state_error;
                    break;
                }
                if (c == ' ') break;
                if (c == ',')
                {
                    state = state_and_register_third;
                    print_token();
                    break;
                }
                add_token(c);
                break;
            case state_and_register_third:
                if (c == '\n' || c == '\r')
                {
                    state = state_next_line;
                    break;
                }
                if (c == ',')
                {
                    state = state_error;
                    break;
                }
                if (c == ' ') break;
                if (c == ';')
                {
                    state = state_start_comment_line;
                    print_token();
                    break;
                }
                add_token(c);
                break;



            /*******************************************
                        // BR OP_CODE //
            **********************************************/


            case state_start_br_parser:
                if (c == '\n' || c == '\r')
                {
                    state = state_error;
                    break;
                }
                if (c == ' ')
                {
                    state = state_get_br_token;
                    print_token();
                    break;
                }
                add_token(c);
                break;
            case state_get_br_token:
                if (c == '\n' || c == '\r')
                {
                    state = state_error;
                    break;
                }
                if (c == ' ') break;
                if (c == ';')
                {
                    state = state_start_comment_line;
                    print_token();
                    break;
                }
                add_token(c);
                break;
            case state_end_br_parser:
                if (c == '\n' || c == '\r')
                {
                    state = state_next_line;
                    break;
                }
                if (c == ';')
                {
                    state = state_start_comment_line;
                    break;
                }


            case state_end_add_parser:
                print_token();
            case state_end_and_parser:
                print_token();
            case state_error:
                printf("error\n");
                return -1;
            default:
                printf("done\n");
        }
    }
}
int main(int argc, char** argv)
{

    const char* code[4] = {
        ".orig x3000  ; Starting address.",
        "lea r0, #3  ; Load address of msg0 into r0",
        "lea r0, #1	; Load address of msg1 into r0."
    };

    return parser();
}

/*

lea r0, msg0  ; Load address of msg0 into r0.
puts          ; Print contents of r0 (msg0).
lea r0, msg1	; Load address of msg1 into r0.
puts          ; Print contents of r0 (msg1 string).

lea r2, neg97   ; Load address of neg97 into r2.
ldr r2, r2, #0  ; Load contents of address in r2, into r2 (r2 now holds -97).
.end

*/
