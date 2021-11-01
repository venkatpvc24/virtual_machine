
#include "assembler.h"

static char line[500];
static vm_token_t token;
static vm_opcode_t opcode_state;
static char* c;
static bool base10 = false;
static bool base16 = false;
static bool is_reg;
static int line_number = 0;

u16 size = 0;


static const int op_codes[17] = {5861625, 193432126, 5861941, 5862188, 193442436, 193432456, 193444119,
                                         193452270, 193451172, 193446662, 193444110, 193452261, 193450688,
                                         193450687, 193444135, 2088990876, -1592907371};

static const int traps[5] = {2088851025, 2088510152, 2088541486, 5861852, 193447949};


vm_state_t vm_next_token(void);

int vm_string_hashing(const char* s)
{
    int hash = 5381;
    while(*s)
    {
        hash = (hash << 5) + hash + *s - '0';
        s++;
    }

    return hash;
}

static void vm_to_lower(char* word) {
    for (size_t i = 0; i < strlen(word); i++) {
        if (isupper(word[i])) word[i] |= 32;
    }
}

int vm_parse_number(int base) {
    vm_next_token();
    char* end;
    char* reg = malloc(sizeof(char) * token.count);
    sprintf(reg, "%.*s", token.count, token.current);
    int number = strtoul(reg, &end, base);
    token.count = 0;
    return number;
}


char* vm_create_string(void) {
    char* dest = malloc(sizeof(char) * token.count + 1);
    sprintf(dest, "%.*s", token.count, token.current);
    vm_to_lower(dest);
    token.count = 0;
    return dest;
}


typedef struct {
    int type;
    int current;
    cPtr opcode;
} op_trap_t;

op_trap_t op_trap;

vm_state_t vm_next_token(void)
{
    while (isspace(*c)) ++c;

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
            base16= false;
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
}



u16 vm_is_register(void) {
    vm_next_token();
    char* reg = malloc(sizeof(char) * token.count);
    sprintf(reg, "%.*s", token.count, token.current);
    token.count = 0;
    //printf("reg: %s\n", reg);
    bool is_register = (strlen(reg) == 2 && (reg[0] == 'r' || reg[0] == 'R') &&
                        (reg[1] >= '0' && reg[1] <= '7'))
                           ? true
                           : false;

    if (!is_register) {
        fprintf(stderr, "expect register, but got this, %s\n", reg);
    }
    return reg[1] - '0';
}

int vm_get_op(cPtr op_code) {
    //vm_to_lower(op_code);
    if (op_code[0] == 'b' && op_code[1] == 'r') return 0;

    for (int i = 0; i < 17; i++) {
        if (vm_string_hashing(op_code) == op_codes[i]) {
            return i;
        }
    }

    return -1;
}



static int vm_get_trap(char* trap) {
    //vm_to_lower(trap);

    for (int i = 0; i < 5; i++) {
        if (vm_string_hashing(trap) == traps[i]) {
            return i;
        }
    }
    return -1;
}


static void find_op_type(void) {
    op_trap.opcode = vm_create_string();
    int op = vm_get_op(op_trap.opcode);

    if (op != -1) {
        op_trap.type = TYPE_OPCODE;
        op_trap.current = op;
        return;
    }

    int trap = vm_get_trap(op_trap.opcode);
    if (trap != -1) {
        op_trap.type = TYPE_TRAP;
        op_trap.current = trap;
        return;
    }

    if (strcmp(op_trap.opcode, "orig") == 0 || strcmp(op_trap.opcode, "end") == 0)
    {
        op_trap.type = TYPE_START_END;
        return;
    }
    op_trap.type = TYPE_LABEL;
}


typedef struct {
    cPtr name;
    int line_number;
} label_t;

void label_init(label_t label)
{
  label.name = NULL;
  label.line_number = 0;
}

u16 find_label(label_t* labels) {
    char* label = vm_create_string();
    vm_to_lower(label);
    if (isdigit(label[0]))
    {
        int base = base10 ? 10 : 16;
        u16 number = vm_parse_number(base);
        return number;
    }
    else
    {

        for (int i = 0; i < 100; i++) {
            if (labels[0].name != NULL) {
                if (strcmp(labels[i].name, label) == 0)
                {
                    return labels[i].line_number - line_number;
                }
            }
         }
      }

    return -1;
}

void assembler(const char* filename, u16* data, u16* len_of_data, u16* start_address)
{
    // fopen("/home/venkatpvc/test.asm", "r");
    FILE* in = fopen(filename, "r");
    if (in == NULL)
    {
      printf("error file could not opened\n");
      exit(EXIT_FAILURE);
    }
    int index = 0;
    //int address = *start_address; // remove later;
    //int len = size; // remove later;
    bool orig_initilized = false;
    int __require;

    label_t labels[100];

    for (int i = 0; i < 100; i++) label_init(labels[i]);

    while (fgets(line, 500, in))
    {
        c = line;
        __require = vm_next_token();


        if (__require == TK_EOL) continue;

        if (__require == TK_DOT) op_trap.type = TYPE_LABEL;
        else
            find_op_type();

        if (op_trap.type != TYPE_LABEL) line_number++; // count lines of opcode, traps except label

        if (op_trap.type == TYPE_LABEL)
        {
            if (__require != TK_DOT)
            {
                labels[index].name = malloc(sizeof(char) * token.count);
                strcpy(labels[index].name, op_trap.opcode);
                labels[index].line_number = line_number == 0 ? line_number + 1 : line_number;
                token.count = 0;
                index++;
            }
            if (__require != TK_DOT)
            {
                //printf("%.*s\n", token.count, token.current);
                token.count = 0;
                __require = vm_next_token();
            }
            if (__require == TK_DOT)
            {
                vm_next_token();
                op_trap.opcode = vm_create_string();
                token.count = 0;

            }
            if (strcmp(op_trap.opcode, "stringz") == 0)
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
            if (strcmp(op_trap.opcode, "fill") == 0)
            {
                line_number++;
            }

            if (strcmp(op_trap.opcode, "blkw") == 0)
            {
              __require = vm_next_token();
                  if (__require != TK_NUMBER) printf("error\n");
                int b = base10 ? 10 : 16;
                u16 value = vm_parse_number(b);

                for (u16 i = 0; i < value; i++) line_number++;
            }
        }
    }


    rewind(in);
    line_number = 0, size = 0;
    while (fgets(line, 500, in))
    {
        c = line;
        //printf("buf: %s\n", c);
        is_reg = true;
        base16 = false;
        base10 = false;
        __require = vm_next_token();
        if (__require == TK_EOL) continue;
        if (__require == TK_DOT) op_trap.type = TYPE_LABEL;
        else
            find_op_type();
        if (line_number > 0 && orig_initilized != true)
        {
            printf("program must start with orig\n");
            exit(EXIT_FAILURE);
        }
        switch (op_trap.type)
        {
        case TYPE_OPCODE:
            //printf("oh it's opcode\n");
            switch (op_trap.current)
            {
            case VM_ADD:
            case VM_AND:
                line_number++;
                {
                    u16 base = 0;

                    base = (op_trap.current << 12) | (vm_is_register() << 9);
                    __require = vm_next_token();
                    if (__require != TK_COMMA) printf("error\n");

                    base |= (vm_is_register() << 6);
                    __require = vm_next_token();
                    if (__require != TK_COMMA) printf("error\n");

                    vm_next_token();

                    if (!is_reg)
                    {
                        int b = 0;
                        if (base10) b = 10;
                        if (base16) b = 16;
                        int imme = vm_parse_number(b);
                        base |= (1 << 5) | (imme & 0x1F);
                    }
                    else
                    {
                        base |= (0 << 5) | (vm_is_register() & 0x07);
                    }

                    __require = vm_next_token();
                    if (__require != TK_EOL) printf("error\n");

                    data[size++] = base;
                }
                break;
            case VM_BR:
                line_number++;
                {
                    u16 base = 0;
                    base = vm_get_op(op_trap.opcode);


                    char c1 = op_trap.opcode[2];
                    char c2 = op_trap.opcode[3];
                    char c3 = op_trap.opcode[4];

                    base = (base << 12);
                    base |= (c1 == 'n' ? (1 << 11) : (0 << 11));
                    base |= (c1 == 'z' || c2 == 'z' ? (1 << 10) : (0 << 10));
                    base |= (c1 == 'p' || c2 == 'p' || c3 == 'p' ? (1 << 9) : (0 << 9));


                    vm_next_token();

                    base |= (find_label(labels) & 0x1FF);
                    data[size++] = base;
                }
                break;
            case VM_LDI:
            case VM_LD:
            case VM_ST:
            case VM_STI:
            case VM_LEA:
            {
                u16 base = 0;
                line_number++;
                //printf("current: %d\n", op_trap.current);
                base = (op_trap.current << 12);

                //printf("base: %d\n", base);
                base |= (vm_is_register() << 9);

                __require = vm_next_token();
                if (__require != TK_COMMA) printf("error\n");

                vm_next_token();
                base |= (find_label(labels) & 0x1FF);


                __require = vm_next_token();
                if (__require != TK_EOL) printf("error\n");

                data[size++] = base;

            }
            break;
            case VM_JSR:
                line_number++;
                {
                    u16 base = (op_trap.current << 12);
                    vm_next_token();
                    base |= (1 << 11) | (find_label(labels) & 0x7FF);

                    __require = vm_next_token();
                    if (__require != TK_EOL) printf("error\n");
                    data[size++] = base;
                }
                break;
            case VM_LDR:
            case VM_STR:
            {
                line_number++;
                u16 base = 0;

                base = (op_trap.current << 12) | (vm_is_register() << 9);
                __require = vm_next_token();
                if (__require != TK_COMMA) printf("error\n");

                base |= (vm_is_register() << 6);

                __require = vm_next_token();
                if (__require != TK_COMMA) printf("error\n");

                vm_next_token();

                base |= (find_label(labels) & 0x3F);


                __require = vm_next_token();
                if (__require != TK_EOL) printf("error\n");

                data[size++] = base;
            }
            }
            opcode_state = VM_EOL;
            break;
        case TYPE_TRAP:
        {
            //printf("oh it's trap\n");
            opcode_state = VM_EOL;
            u16 base = 0xF000;
            if (strcmp(op_trap.opcode, "getc") == 0) base |= (0x20 & 0xFF);
            if (strcmp(op_trap.opcode, "out") == 0) base |= (0x21 & 0xFF);
            if (strcmp(op_trap.opcode, "puts") == 0) base |= (0x22 & 0xFF);
            if (strcmp(op_trap.opcode, "in") == 0) base |= (0x23 & 0xFF);
            if (strcmp(op_trap.opcode, "halt") == 0) base |= (0x25 & 0xFF);
            data[size++] = base;
            line_number++;
        }
        break;
        case TYPE_LABEL:
            if (__require != TK_DOT)
            {
                token.count = 0;
                __require = vm_next_token();
            }
            if (__require == TK_DOT)
            {
                vm_next_token();
                op_trap.opcode = vm_create_string();
                //printf("opcode: %s\n", op_trap.opcode);
                token.count = 0;
            }
            if (strcmp(op_trap.opcode, "orig") == 0)
            {

              __require = vm_next_token();
              if (__require != TK_NUMBER) printf("error\n");
                int b = 0;
                if (base10) b = 10;
                if (base16) b = 16;
                u16 value = vm_parse_number(b);
                *start_address = value;
                size = *start_address;
                orig_initilized = true;
                __require = vm_next_token();
                if (__require != TK_EOL) printf("error\n");
            }
            if (strcmp(op_trap.opcode, "end") == 0)
            {
                //printf("size: %d\n", size);
                *len_of_data = size;
                return;
            }
            if (strcmp(op_trap.opcode, "fill") == 0)
            {
              __require = vm_next_token();
              if (__require != TK_NUMBER) printf("error\n");
                int b = 0;
                if (base10) b = 10;
                if (base16) b = 16;
                //int imme = vm_parse_number(b);
                u16 value = vm_parse_number(b);
                data[size++] = value;
                line_number = size;
                __require = vm_next_token();
                if (__require != TK_EOL) printf("error\n");
            }

            if (strcmp(op_trap.opcode, "blkw") == 0)
            {
              __require = vm_next_token();
              if (__require != TK_NUMBER) printf("error\n");
                int b = 0;
                if (base10) b = 10;
                if (base16) b = 16;
                //int imme = vm_parse_number(b);
                u16 value = vm_parse_number(b);
                for (u16 i = 0; i < value; i++) data[size++] = 0;
                line_number = size;


                __require = vm_next_token();
                if (__require != TK_EOL) printf("error\n");
            }

            if (strcmp(op_trap.opcode, "stringz") == 0)
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

                    data[size++] = *c;
                    c++;
                }
                data[size++] = '\0';
                line_number = size;

                __require = vm_next_token();
                if (__require != TK_EOL) printf("error\n");
            }
            break;
        }
    }

}
