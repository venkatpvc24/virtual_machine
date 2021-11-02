#include "../include/opcode.h"

extern u16 line_number, size;
extern u16 data[USHRT_MAX];
extern int __require;
extern bool base10, base16, is_reg;


extern label_t labels;

static void assembler_add_and(void)
{

    line_number++;
    u16 base = 0;

    base = (parser.current << 12) | (vm_is_register() << 9);
    __require = vm_next_token();
    if (__require != TK_COMMA)
        printf("error\n");

    base |= (vm_is_register() << 6);
    __require = vm_next_token();
    if (__require != TK_COMMA)
        printf("error\n");

    vm_next_token();

    if (!is_reg)
    {
        int b = 0;
        if (base10)
            b = 10;
        if (base16)
            b = 16;
        int imme = vm_parse_number(b);
        base |= (1 << 5) | (imme & 0x1F);
    }
    else
    {
        base |= (0 << 5) | (vm_is_register() & 0x07);
    }

    __require = vm_next_token();
    if (__require != TK_EOL)
        printf("error\n");

    data[size++] = base;
}

static void assembler_branch(void)
{
    line_number++;
    u16 base = 0;
    base = vm_get_op(parser.opcode);

    char c1 = parser.opcode[2];
    char c2 = parser.opcode[3];
    char c3 = parser.opcode[4];

    base = (base << 12);
    base |= (c1 == 'n' ? (1 << 11) : (0 << 11));
    base |= (c1 == 'z' || c2 == 'z' ? (1 << 10) : (0 << 10));
    base |= (c1 == 'p' || c2 == 'p' || c3 == 'p' ? (1 << 9) : (0 << 9));

    vm_next_token();

    base |= (find_label() & 0x1FF);
    data[size++] = base;
}

//   case VM_LD, case VM_ST, case VM_STI, case VM_LE

static void assembler_load(void)
{
    u16 base = 0;
    line_number++;
    base = (parser.current << 12);

    base |= (vm_is_register() << 9);

    __require = vm_next_token();
    if (__require != TK_COMMA)
        printf("error\n");

    vm_next_token();
    base |= (find_label() & 0x1FF);

    __require = vm_next_token();
    if (__require != TK_EOL)
        printf("error\n");

    data[size++] = base;
}

// case VM_LDR, case VM_STR:

static void assembler_load_store(void)
{
    line_number++;
    u16 base = 0;

    base = (parser.current << 12) | (vm_is_register() << 9);
    __require = vm_next_token();
    if (__require != TK_COMMA)
        printf("error\n");

    base |= (vm_is_register() << 6);

    __require = vm_next_token();
    if (__require != TK_COMMA)
        printf("error\n");

    vm_next_token();

    base |= (find_label() & 0x3F);

    __require = vm_next_token();
    if (__require != TK_EOL)
        printf("error\n");

    data[size++] = base;
}

static void assembler_jsr(void)

{
    line_number++;
    u16 base = (parser.current << 12);
    vm_next_token();
    base |= (1 << 11) | (find_label() & 0x7FF);

    __require = vm_next_token();
    if (__require != TK_EOL)
        printf("error\n");
    data[size++] = base;
}


/*
+63static const char* op_codes[17] = {"br",  "add", "ld",  "st",   "jsr",   "and",
                                   "ldr", "str", "rti", "not",  "ldi",   "sti",
                                   "ret", "res", "lea", "trap", "pesudo"
 */

void opcode_parser(void)
{
    is_reg = true;
    const opcode_t opcodes[17] = 
    {
        {"br", 5861625, assembler_branch},
        {"add", 193432126, assembler_add_and},
        {"ld", 5861941, assembler_load},
        {"st", 5862188, assembler_load},
        {"jsr", 193442436, assembler_jsr},
        {"and", 193432456, assembler_add_and},
        {"ldr", 193444119, assembler_load_store},
        {"str", 193452270, assembler_load_store},
        {"not", 193446662, assembler_load}, // needed implement
        {"ldi", 193444110, assembler_load},
        {"sti", 193452261, assembler_load},
        {"ret", 193450688, assembler_load}, // needed implemnt
        {"lea", 193444135, assembler_load}
    };

    for(int i = 0; i < 17; i++)
    {
        if (vm_string_hashing(parser.opcode) == opcodes[i].type)
        {
            (opcodes[i].fPtr)();
            break;
        }
    }
}