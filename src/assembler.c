
#include "../include/assembler.h"
#include "../include/label.h"
#include "../include/opcode.h"
#include "../include/trap.h"


char* c;
bool base10 = false;
bool base16 = false;
int __require = 0;
bool is_orig_initialized = false;
bool is_reg = false;
int line_number = 0;
u16 size = 0;
u16 start_address = 0;

label_t labels[100];

extern u16 data[USHRT_MAX];

const int op_codes[17] = {5861625, 193432126, 5861941, 5862188, 193442436, 193432456, 193444119,
                                         193452270, 193451172, 193446662, 193444110, 193452261, 193450688,
                                         193450687, 193444135, 2088990876, -1592907371};


// static const char* traps[5] = {"getc", "out", "puts", "in", "halt"};

const int traps[] = {2088510152, 193447949, 2088851025, 5861852, 2088541486};


int vm_string_hashing(const char *s)
{
    int hash = 5381;
    while (*s)
    {
        hash = (hash << 5) + hash + *s - '0';
        s++;
    }

    return hash;
}

void vm_to_lower(char *word)
{
    for (size_t i = 0; i < strlen(word); i++)
    {
        if (isupper(word[i]))
            word[i] |= 32;
    }
}

int vm_parse_number(int base)
{
    vm_next_token();
    char *end;
    char *reg = malloc(sizeof(char) * token.count);
    sprintf(reg, "%.*s", token.count, token.current);
    int number = strtoul(reg, &end, base);
    token.count = 0;
    return number;
}

char *vm_create_string(void)
{
    char *dest = malloc(sizeof(char) * token.count + 1);
    sprintf(dest, "%.*s", token.count, token.current);
    vm_to_lower(dest);
    token.count = 0;
    return dest;
}

vm_state_t vm_next_token(void)
{
    while (*c == ' ') ++c;

    switch (*c)
    {
    case '.':
        c++;
        token.current = c;
        return TK_DOT;
    case '"':
        c++;
        token.current = c;
        return TK_QUOTES;
    case ',':
        c++;
        token.current = c;
        return TK_COMMA;
    /*case '.':

        op_trap.type = TYPE_ORIG;
        opcode_state = VM_START;
        break;*/
    case ';':
    case '\0':
    case '\n':
        return TK_EOL;
    case '\'':
        c++;
        token.current = c;
        token.count = 0;
        break;
    case 'x':
        if (*c == 'x')
        {
            base16 = true;
            base10 = false;
            is_reg = false;
            c++;
            token.current = c;
        }
        return TK_NUMBER;
    case '#':
        if (*c == '#')
        {
            base10 = true;
            base16 = false;
            is_reg = false;
            c++;
            token.current = c;
        }
        return TK_NUMBER;
    default:
    {
        token.current = c;
        if (*c == '-')
        {
            token.count++;
            c++;
        }
        if (*c == '.')
        {
            c++;
            token.current = c;
        }
        while (isalnum(*c))
        {
            c++;
            token.count++;
        }
        opcode_state = TK_ALPHA;
    }
    }
    return -1;
}

u16 vm_is_register(void)
{
    vm_next_token();
    char *reg = malloc(sizeof(char) * token.count);
    sprintf(reg, "%.*s", token.count, token.current);
    token.count = 0;
    bool is_register = (strlen(reg) == 2 && (reg[0] == 'r' || reg[0] == 'R') &&
                        (reg[1] >= '0' && reg[1] <= '7'))
                           ? true
                           : false;

    if (!is_register)
    {
        fprintf(stderr, "expect register, but got this, %s\n", reg);
    }
    return reg[1] - '0';
}

int vm_get_op(cPtr op_code)
{
    if (op_code[0] == 'b' && op_code[1] == 'r')
        return 0;

    for (int i = 0; i < 17; i++)
    {
        if (vm_string_hashing(op_code) == op_codes[i])
        {
            return i;
        }
    }

    return -1;
}

int vm_get_trap(char *trap)
{
    for (int i = 0; i < 5; i++)
    {
        if (vm_string_hashing(trap) == traps[i])
        {
            return i;
        }
    }
    return -1;
}

void find_op_type(void)
{
    parser.opcode = vm_create_string();
    int op = vm_get_op(parser.opcode);

    if (op != -1)
    {
        parser.type = TYPE_OPCODE;
        parser.current = op;
        return;
    }

    int trap = vm_get_trap(parser.opcode);
    if (trap != -1)
    {
        parser.type = TYPE_TRAP;
        parser.current = trap;
        return;
    }

    if (strcmp(parser.opcode, "orig") == 0 || strcmp(parser.opcode, "end") == 0)
    {
        parser.type = TYPE_START_END;
        return;
    }
    parser.type = TYPE_LABEL;
}

void label_init(label_t label)
{
    label.name = NULL;
    label.line_number = 0;
}

u16 find_label(void)
{
    char *label = vm_create_string();
    if (isdigit(label[0]))
    {
        int base = base10 ? 10 : 16;
        u16 number = vm_parse_number(base);
        return number;
    }
    else
    {

        for (int i = 0; i < 100; i++)
        {
            if (labels[i].name != NULL)
            {
                if (strcmp(labels[i].name, label) == 0)
                {
                    return labels[i].line_number - line_number;
                }
            }
        }
    }

    return -1;
}

void assembler(const char *filename, u16 *len_of_data)
{
    FILE *in = fopen(filename, "r");
    if (in == NULL)
    {
        printf("error file could not opened\n");
        exit(EXIT_FAILURE);
    }
    int index = 0;
    char line[500];
    
    collect_labels(index, in, line);

    rewind(in);
    line_number = 0, size = 0;

    while (fgets(line, 500, in))
    {
        c = line;
        __require = vm_next_token();
        if (__require == TK_EOL)
            continue;
        if (__require == TK_DOT)
            parser.type = TYPE_LABEL;
        else
            find_op_type();
        if (line_number > 0 && is_orig_initialized != true)
        {
            printf("program must start with orig\n");
            exit(EXIT_FAILURE);
        }
        switch (parser.type)
        {
            case TYPE_OPCODE:
                opcode_parser();
                break;
            case TYPE_TRAP:
                trap_parser();
                break;
            case TYPE_LABEL:
                label_pesudo_parser();
                break;
        }

        *len_of_data = size;
    }

    //for (int i = 0x3000; i < size; i++) printf("0x%04x, ", data[i]);
}