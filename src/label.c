#include "../include/assembler.h"
#include "../include/label.h"


extern u16 line_number, size, start_address;
extern u16 data[USHRT_MAX];
extern int __require;
extern bool base10, base16, is_reg, is_orig_initialized;
extern char* c;
extern const char* traps;
extern label_t labels[100];

static bool is_label_exists(char* label)
{
    for (int i = 0; i < 100; i++)
    {
        if (labels[i].name != NULL)
        {
            if (strcmp(label, labels[i].name) == 0)
            {
                return true;
            }
        }

    }

    return false;
}

void collect_labels(int index, FILE* in, char* line)
{
    
    for (int i = 0; i < 100; i++) label_init(labels[i]);

    while (fgets(line, 500, in))
    {
        c = line;
        __require = vm_next_token();


        if (__require == TK_EOL) continue;

        if (__require == TK_DOT) parser.type = TYPE_LABEL;
        else
            find_op_type();

        if (parser.type != TYPE_LABEL) line_number++; // count lines of opcode, traps except label

        if (parser.type == TYPE_LABEL)
        {
            if (__require != TK_DOT)
            {
                if (is_label_exists(parser.opcode) == true)
                    printf("label already exists - %s\n", parser.opcode);

                labels[index].name = malloc(sizeof(char) * token.count);
                strcpy(labels[index].name, parser.opcode);
                labels[index].line_number = line_number == 0 ? line_number + 1 : line_number;
                token.count = 0;
                index++;
            }
            if (__require != TK_DOT)
            {
                token.count = 0;
                __require = vm_next_token();
            }
            if (__require == TK_DOT)
            {
                vm_next_token();
                parser.opcode = vm_create_string();
                token.count = 0;

            }
            if (strcmp(parser.opcode, "stringz") == 0)
            {
              __require = vm_next_token();
                  if (__require != TK_QUOTES) printf("error\n");
                while (*c)
                {
                    if (*c == '"')
                    {
                        c++;
                        continue;
                    }
                    if (*c == '\n') break;
                    c++;
                    size++;
                }
                line_number += size++;
            }
            if (strcmp(parser.opcode, "fill") == 0)
            {
                line_number++;
            }

            if (strcmp(parser.opcode, "blkw") == 0)
            {
              __require = vm_next_token();
                  if (__require != TK_NUMBER) printf("error\n");
                int b = base10 ? 10 : 16;
                u16 value = vm_parse_number(b);

                for (u16 i = 0; i < value; i++) line_number++;
            }
        }
    }
}

static void assemble_orig(void)
{
        __require = vm_next_token();
        if (__require != TK_NUMBER)
            printf("error\n");
        int b = 0;
        if (base10) b = 10;
        if (base16) b = 16;
        u16 value = vm_parse_number(b);
        start_address = value;
        size = start_address;
        is_orig_initialized = true;
        __require = vm_next_token();
        if (__require != TK_EOL)
            printf("error\n");
}

static void assemble_fill(void)
{
    __require = vm_next_token();
        if (__require != TK_NUMBER)
            printf("error\n");
        int b = 0;
        if (base10) b = 10;
        if (base16) b = 16;
        u16 value = vm_parse_number(b);
        data[size++] = value;
        line_number = size;
        __require = vm_next_token();
        if (__require != TK_EOL)
            printf("error\n");
}


static void assemble_blkw(void)
{
    __require = vm_next_token();
        if (__require != TK_NUMBER)
            printf("error\n");
        int b = 0;
        if (base10) b = 10;
        if (base16) b = 16;
        u16 value = vm_parse_number(b);
        for (u16 i = 0; i < value; i++)
            data[size++] = 0;
        line_number = size;

        __require = vm_next_token();
        if (__require != TK_EOL)
            printf("error\n");
}

static void assemble_stringz(void)
{
    __require = vm_next_token();
        if (__require != TK_QUOTES)
            printf("error\n");
        while (*c)
        {
            if (*c == '"')
            {
                c++;
                continue;
            }
            if (*c == '\n') break;

            data[size++] = *c;
            c++;
        }
        data[size++] = '\0';
        line_number = size;

        __require = vm_next_token();
        if (__require != TK_EOL)
            printf("error\n");
}


static const pesudo_t pesudo[5] = {
        {"orig", 1, assemble_orig},
        {"fill", 2, assemble_fill},
        {"blkw", 3, assemble_blkw},
        {"stringz", 4, assemble_stringz}
        };

void label_pesudo_parser(void)
{

    if (__require != TK_DOT)
    {
        token.count = 0;
        __require = vm_next_token();
    }
    if (__require == TK_DOT)
    {
        vm_next_token();
        parser.opcode = vm_create_string();
        token.count = 0;
    }

    if (strcmp(parser.opcode, "end") == 0)
    {
        return;
    }
    for (int i = 0; i < 5; i++) 
    {
        if (strcmp(parser.opcode, pesudo[i].name) == 0) 
        {
            (pesudo[i].fPtr)();
            break;
        }
    }


}