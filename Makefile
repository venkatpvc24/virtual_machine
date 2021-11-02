INCLUDE := ./include
SRC := ./src
OBJ := ./obj
SRCS := $(wildcard $(SRC)/*.c)

CC := gcc #gcc

CFLAGS := -std=c11 -g -Wall -Wextra -Wpedantic  -Wformat=2  \
											-Wno-unused-parameter -Wshadow -Wwrite-strings -Wstrict-prototypes -Wold-style-definition  \
											-Wredundant-decls -Wnested-externs -Wmissing-include-dirs -O2

# debug := CFLAGS += -g

objects = main.o trap.o opcode.o label.o assembler.o vm.o 


main debug: main.o trap.o opcode.o label.o assembler.o vm.o
	$(CC) $(CFLAGS) -o $@ $^ -Wall -O2



clean:
	@ find -name "*.o" -type f -delete
	@ echo "deleted all files with extension '.o'"