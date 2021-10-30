
#include "assembler.h"

static char line[500];
static vm_token_t token;
static vm_opcode_t opcode_state;
static vm_types_t types_state;
static char* c;
static bool base10 = false;
static bool base16 = false;
static bool is_reg;
static int line_number = 0;

u16 size = 0;


static const char* op_codes[17] = {"br",  "add", "ld",  "st",   "jsr",   "and",
                                   "ldr", "str", "rti", "not",  "ldi",   "sti",
                                   "ret", "res", "lea", "trap", "pesudo"};

static const char* traps[5] = {"puts", "getc", "halt", "in", "out"};


vm_state_t vm_next_token();

static void vm_to_lower(char* token) {
    for (size_t i = 0; i < strlen(token); i++) {
        if (isupper(token[i])) token[i] |= 32;
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

void vm_copy_string(char** dest) {
    *dest = malloc(sizeof(char) * token.count);
    sprintf(*dest, "%.*s", token.count, token.current);
    token.count = 0;
}

char* vm_create_string() {
    char* dest = malloc(sizeof(char) * token.count + 1);
    sprintf(dest, "%.*s", token.count, token.current);
    token.count = 0;
    return dest;
}


int vm_get_op();
static void find_op_type();


typedef struct {
    int type;
    int current;
    cPtr opcode;
} op_trap_t;

op_trap_t op_trap;

vm_state_t vm_next_token() {
    while (isspace(*c)) ++c;

    switch (*c) {
        /*case '.':
            c++;
            token.current = c;
            return TK_DOT;*/
        case ',':
            c++;
            token.current = c;
            return TK_COMMA;
        case '.':
            c++;
            op_trap.type = TYPE_ORIG;
            opcode_state = VM_START;
            break;
        case '\0':
        case ';':
            opcode_state = VM_EOL;
            break;
        case '\'':
        case '"':
            c++;
            token.current = c;
            token.count = 0;
            break;
        case 'x':
            if (*c == 'x') {
                base16 = true;
                base10 = false;
                is_reg = false;
                c++;
                token.current = c;
            }
            return TK_NUMBER;
        case '#':
            if (*c == '#') {
                base10 = true;
                base16= false;
                is_reg = false;
                c++;
                token.current = c;
            }
            return TK_NUMBER;
        default: {
            token.current = c;
            if (*c == '-') {
                token.count++;
                c++;
            }
            while (isalnum(*c)) {
                c++;
                token.count++;
            }
            opcode_state = TK_ALPHA;
        }
    }
}

void vm_require(vm_state_t state) {
    if (vm_next_token() != state) {
        fprintf(stderr, "syntax error, expects comma after register\n");
    }
}

u16 vm_is_register() {
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
    vm_to_lower(op_code);
    if (op_code[0] == 'b' && op_code[1] == 'r') return 0;

    for (int i = 0; i < 17; i++) {
        if (strcmp(op_code, op_codes[i]) == 0) {
            return i;
        }
    }

    return -1;
}



static int vm_get_trap(char* trap) {
    vm_to_lower(trap);

    for (int i = 0; i < 5; i++) {
        if (strcmp(trap, traps[i]) == 0) {
            return i;
        }
    }
    return -1;
}


static void find_op_type() {
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
    }
    op_trap.type = TYPE_LABEL;
}


typedef struct {
    cPtr name;
    int line_number;
} label_t;

label_t* label_init() {
    label_t* lbl = malloc(sizeof *lbl);
    lbl->name = NULL;
    lbl->line_number = 1;
    return lbl;
}

label_t* labels[100];



u16 find_label() {
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
            if (labels[0]->name != NULL) {
                if (strcmp(labels[i]->name, label) == 0)
                {
                    return labels[i]->line_number - line_number;
                }
            }
         }
      }

    return -1;
}


//int assembler(u16* data, u16 len_of_data, u16 start_address);

int assembler(const char* filename, u16* data, u16* len_of_data, u16* start_address) {
    // fopen("/home/venkatpvc/test.asm", "r");
    FILE* in = fopen(filename, "r");
    int index = 0;
    int address = *start_address; // remove later;
    int len = size; // remove later;
    bool orig_initilized = false;

    for (int i = 0; i < 100; i++)
    {
        labels[i] = label_init();
    }

    while (fgets(line, 500, in)) {
        c = line;

        if (*c == '.') continue;

        vm_next_token();
        find_op_type();
        if (opcode_state == VM_EOL) continue;
        if (op_trap.type != TYPE_LABEL) line_number++;

        if (op_trap.type == TYPE_LABEL)
        {
            labels[index]->name = malloc(sizeof(char) * token.count);
            strcpy(labels[index]->name, op_trap.opcode);
            labels[index]->line_number = line_number == 0 ? line_number + 1 : line_number;
            index++;
            vm_next_token();
            token.count = 0;
            if (*c == 's' || *c == 'S')
             {
                vm_next_token();
                char stringz[7];

                sprintf(stringz, "%.*s", token.count, token.current);
                vm_to_lower(stringz);
                token.count = 0;

                if (strcmp(stringz, "stringz") == 0)
                {
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
                    line_number += size;
                }
            }
        }
    }


    rewind(in);
    line_number = 0, size = 0;
    while (fgets(line, 500, in)) {
        c = line;
        //printf("buf: %s\n", c);
        vm_next_token();
        find_op_type();
        if (opcode_state == VM_EOL) continue;

        if (!orig_initilized) op_trap.type = TYPE_ORIG;

        switch (op_trap.type) {
            case TYPE_ORIG:
                {
                    vm_next_token();
                    char orig[5];
                    sprintf(orig, "%.*s", token.count, token.current);
                    vm_to_lower(orig);
                    token.count = 0;
                    if (strcmp(orig, "orig") == 0)
                    {
                        orig_initilized = true;
                        vm_require(TK_NUMBER);
                        vm_next_token();
                        *start_address = vm_parse_number(16);
                        size = *start_address;
                        //address = *start_address;
                        vm_next_token();

                        if (opcode_state != VM_EOL) {
                            fprintf(stderr, "syntax error\n");
                        }

                    }
                    else
                    {
                        fprintf(stderr, "program must start with orig\n");
                    }
                }
                break;
            case TYPE_OPCODE:
                //printf("oh it's opcode\n");
                switch (op_trap.current) {
                    case VM_ADD:
                    case VM_AND:
                        line_number++;
                        {
                            u16 base = 0;

                            if (base == -1) {
                                fprintf(stderr, "invalid opcode - %s", op_codes[base]);
                            }

                            base = (1 << 12) | (vm_is_register() << 9);
                            vm_require(TK_COMMA);

                            base |= (vm_is_register() << 6);
                            vm_require(TK_COMMA);

                            vm_next_token();

                            if (!is_reg) {
                                int b;
                                if (base10) b = 10;
                                if (base16) b = 16;
                                int imme = vm_parse_number(b);
                                base |= (1 << 5) | (imme & 0x1F);
                            } else {
                                base |= (0 << 5) | (vm_is_register() & 0x1F);
                            }

                            vm_next_token();

                            if (opcode_state != VM_EOL) {
                                fprintf(stderr, "syntax error\n");
                            }
                            data[size++] = base;
                        }
                        break;
                    case VM_BR:
                        line_number++;
                        {
                          u16 base = 0;
                          base = vm_get_op(op_trap.opcode);
                          //printf("base: %s\n", op_trap.opcode);


                          if (base == -1) {
                              fprintf(stderr, "invalid opcode - %s", op_codes[base]);
                          }

                          char c1 = op_trap.opcode[2];
                          char c2 = op_trap.opcode[3];
                          char c3 = op_trap.opcode[4];

                          base = (base << 12);
                          base |= (c1 == 'n' ? (1 << 11) : (0 << 11));
                          base |= (c1 == 'z' || c2 == 'z' ? (1 << 10) : (0 << 10));
                          base |= (c1 == 'p' || c2 == 'p' || c3 == 'p' ? (1 << 9) : (0 << 9));


                          vm_next_token();

                          base |= (find_label() & 0x1FF);
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

                        vm_require(TK_COMMA);

                        vm_next_token();
                        base |= (find_label() & 0x1FF);

                        vm_next_token();

                        if (opcode_state != VM_EOL) {
                            fprintf(stderr, "syntax error\n");
                        }

                        data[size++] = base;

                        }
                        break;
                    case VM_JSR:
                        line_number++;
                        {
                            u16 base = (op_trap.current << 12);
                            vm_next_token();
                            base |= (1 << 11) | (find_label() & 0x7FF);
                            vm_next_token();

                            if (opcode_state != VM_EOL) {
                                fprintf(stderr, "syntax error\n");
                            }
                            data[size++] = base;
                        }
                        break;
                    case VM_LDR:
                    case VM_STR:
                        {
                            line_number++;
                            u16 base = 0;

                            base = (op_trap.current << 12) | (vm_is_register() << 9);
                            vm_require(TK_COMMA);

                            base |= (vm_is_register() << 6);
                            vm_require(TK_COMMA);

                            vm_next_token();

                            base |= (find_label() & 0x3F);

                            vm_next_token();

                            if (opcode_state != VM_EOL) {
                                fprintf(stderr, "syntax error\n");
                            }

                            data[size++] = base;
                        }
                }
                opcode_state = VM_EOL;
                break;
            case TYPE_TRAP:
                {
                printf("oh it's trap\n");
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
                vm_next_token();
                vm_next_token();
                    char stringz[7];
                    sprintf(stringz, "%.*s", token.count, token.current);
                    vm_to_lower(stringz);
                    token.count = 0;

                    if (strcmp(stringz, "fill") == 0)
                    {
                      vm_next_token();
                      int b;
                      if (base10) b = 10;
                      if (base16) b = 16;
                      //int imme = vm_parse_number(b);
                      u16 value = vm_parse_number(b);
                      data[size++] = value;
                      line_number += size;
                      vm_next_token();

                      if (opcode_state != VM_EOL) {
                          fprintf(stderr, "syntax error\n");
                      }
                    }

                    if (strcmp(stringz, "blkw") == 0)
                    {
                      vm_next_token();
                      int b;
                      if (base10) b = 10;
                      if (base16) b = 16;
                      //int imme = vm_parse_number(b);
                      u16 value = vm_parse_number(b);
                      for (u16 i = 0; i < value; i++) data[size++] = 0;
                      line_number += size;

                      vm_next_token();

                      if (opcode_state != VM_EOL) {
                          fprintf(stderr, "syntax error\n");
                      }
                    }

                    if (strcmp(stringz, "stringz") == 0)
                    {
                            vm_next_token();
                             while (*c) {
                                if (*c == '"') {
                                    c++;
                                    continue;
                                }
                                if (*c == '\n') break;

                                data[size++] = *c;
                                c++;
                            }
                            data[size++] = '\0';
                            line_number += size;

                            vm_next_token();

                            if (opcode_state != VM_EOL) {
                                fprintf(stderr, "syntax error\n");
                            }
                    }
                opcode_state = VM_EOL;
                break;
            defualt:
                printf("default done\n");
        }
    }
    *len_of_data = size;
    //printf("n: %d\n", n);
    for (int i = 0x3000; i < 0x3020; i++) printf("0x%04x, ", data[i]);
}
