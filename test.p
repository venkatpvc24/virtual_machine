LC-3 instruction_set

LD DR, address             // loads the value from from address @DR(destination register);
LDI DR, address            // loads data @DR from (address) which stores the address to get data from.












all: $(objects)
	$(CC) -o main $(objects)
	@ echo "main executable created\n"

debug: $(objects)
	$(CC) -g -o main $(objects)
	@ echo "debug executable created\n"

main.o: main.c
	$(CC) $(CFLAGS) -g -c main.c
	@ echo "objet main.o created\n"

$(OBJ)/add.o: ./src/add.c
	$(CC) $(CFLAGS) -g -c ./src/add.c -o $(OBJ)/add.o
	@ echo "objet add.o created\n"

utilities.o: utilities.c
	$(CC) $(CFLAGS) -g -c utilities.c







#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

//-DSIZE=70 -Wall -Wextra -Werror -Wpedantic -std=c99 -fsanitize=undefined -fsanitize=address

typedef struct
{
    int sp;
    int ip;
    int* stack;
    int* code;
} vm_t;


typedef enum
{
    PSH,
    ADD,
    SUB,
    MUL,
    EQ,
    LS,
    POP,
    SET,
    HLT
} instruction_set;

vm_t* new_vm(int* data, int size)
{
    vm_t* vm = (vm_t*)malloc(sizeof *vm);
    vm->ip = 0;
    vm->sp = -1;
    vm->stack = malloc(sizeof(int) * 250);
    vm->code = data;

    return vm;
}


int main()
{
    int program[19] = {
        PSH, 5,
        PSH, 6,
        ADD,
        POP,
        PSH, 10,
        PSH, 20,
        MUL,
        POP,
        PSH, 10,
        PSH, 20,
        SUB,
        POP,
        HLT
    };

    vm_t* vm = new_vm(program, 13);

    bool running = true;

    int ip = 0, sp = -1;
    int stack[250];

    while(running)
    {

        switch(vm->code[vm->ip])
        {
            case PSH:
            {
                vm->sp++;
                vm->stack[vm->sp] = vm->code[++vm->ip];
                break;
            }
            case ADD:
            {
                int a = vm->stack[vm->sp--];
                int b = vm->stack[vm->sp--];
                int val = a + b;
                vm->stack[++vm->sp] = val;
                // printf("%d %d\n", a, b);
                break;
            }
            case MUL:
            {
                int a = vm->stack[vm->sp--];
                int b = vm->stack[vm->sp--];
                int val = a * b;
                vm->stack[++vm->sp] = val;
                break;
            }
            case SUB:
            {
                int a = vm->stack[vm->sp--];
                int b = vm->stack[vm->sp--];
                int val = a - b;
                vm->stack[++vm->sp] = val;
                break;
            }
            case EQ:
            {

            }
            case POP:
                {
                    int val_popped = vm->stack[vm->sp--];
                    printf("popped value from stack-> %d\n", val_popped);
                    break;
                }
            case HLT:
                running = false;
                break;
        }

        vm->ip++;
    }

}




MKDIR   := md
RMDIR   := rd /S /Q
CC      := gcc
BIN     := ./bin
OBJ     := ./obj
INCLUDE := ./include
SRC     := ./src
SRCS    := $(wildcard $(SRC)/*.c)
OBJS    := $(patsubst $(SRC)/%.c,$(OBJ)/%.o,$(SRCS))
EXE     := $(BIN)/main.exe
CFLAGS  := -I$(INCLUDE)
LDLIBS  := -lm

.PHONY: all run clean

all: $(EXE)

$(EXE): $(OBJS) | $(BIN)
    $(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)

$(OBJ)/%.o: $(SRC)/%.c | $(OBJ)
    $(CC) $(CFLAGS) -c $< -o $@

$(BIN) $(OBJ):
    $(MKDIR) $@

run: $(EXE)
    $<

clean:
    $(RMDIR) $(OBJ) $(BIN)





    /*case JMP:
                        {
                            printf("i am hear in JMP with 0x%x\n", code);
                            reg_t base = (code >> 8) & 0x07;
                            __memory[R_PC] = base;
                        }
                        break;
                    case JSR:
                        {
                            printf("i am hear in JSR with 0x%x\n", code);
                            uint16_t mode = (code >> 11) & 0x01;
                            uint16_t address = code & 0x7FF;
                            printf("flag: %d\n", mode);
                            __memory[R7] = __memory[R_PC]++;
                            if (mode)
                            {
                                __memory[R_PC] = R0;
                            }
                            else
                            {
                                __memory[R_PC] += address;
                            }
                            // setcc
                        }
                        break;
                    case LDI:
                        {
                            printf("i am hear in LDI with 0x%x\n", code);
                            reg_t dest = (code >> 9) & 0x07;
                            uint16_t address = code & 0xFF;
                            __memory[dest] = __memory[__memory[__memory[R_PC] + address]];
                            // setcc()
                        }
                        break;
                    case LDR:
                        {
                            printf("i am hear in LDR with 0x%x\n", code);
                            reg_t dest = (code >> 9) & 0x07;
                            reg_t base = (code >> 6) & 0x07;
                            reg_t src = code & 0x1F;

                            __memory[dest] = __memory[base + src];

                            // setcc
                        }
                        break;
                    case ST:
                        {
                            printf("i am hear in ST with 0x%x\n", code);
                            reg_t r0 = (code >> 9) & 0x7;
                            uint16_t address = code & 0x1FF;
                            __memory[address] = __memory[r0];

                        }
                        break;
                    case STI:
                        {
                            printf("i am hear in STI with 0x%x\n", code);
                            reg_t r0 = (code >> 9) & 0x7;
                            uint16_t address = code & 0x1FF;

                            __memory[__memory[R_PC + address]] = __memory[r0];
                        }
                        break;
                    case STR:
                        {
                            printf("i am hear in STR with 0x%x\n", code);
                            reg_t r0 = (code >> 9) & 0x7;
                            uint16_t address = code & 0x1F;
                            reg_t r1 = (code >> 6) & 0x7;

                            __memory[r1 + address] = __memory[r0];

                        }
                        break;
                    case TRAP:
                        {
                            printf("i am hear in TRAP with 0x%x\n", code);
                            //__memory[R7] = __memory[R_PC];
                            uint16_t trap = code & 0xFF;
                            __memory[R_PC] = __memory[trap];
                        }
                        break;*/
