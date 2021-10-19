INCLUDE := ./include
SRC := ./src
OBJ := ./obj
SRCS := $(wildcard $(SRC)/*.c)

CC := gcc
CFLAGS := -I$(INCLUDE) -std=c11 -Wall -Wextra -Wpedantic  -Wformat=2  \
-Wno-unused-parameter -Wshadow -Wwrite-strings -Wstrict-prototypes -Wold-style-definition  \
-Wredundant-decls -Wnested-externs -Wmissing-include-dirs -O2

debug := CFLAGS += -g

objects = main.o assembler.o vm.o


main debug: main.o assembler.o vm.o
	$(CC) $(CFLAGS) -o $@ $^ -Wall -O2 -D PRINT_CODE=1


clean:
	rm -f /p/a/t/h main main.o debug debug.o assembler.o vm.o
