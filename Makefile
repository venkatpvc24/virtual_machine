INCLUDE := ./include
SRC := ./src
OBJ := ./obj
SRCS := $(wildcard $(SRC)/*.c)

CC := gcc
CFLAGS := -I$(INCLUDE)

debug := CFLAGS += -g

objects = main.o $(OBJ)/add.o utilities.o


$(OBJ)/add.o: ./src/add.c
	$(CC) $(CFLAGS) -g -c ./src/add.c -o $(OBJ)/add.o
	@ echo "objet add.o created\n"

main debug: main.o $(OBJ)/add.o utilities.o
	$(CC) $(CFLAGS) -o $@ $^ -Wall -O2 -D PRINT_CODE=1

$(OBJ)/add.o: $(INCLUDE)/add.h
utilities.o: utilities.h


clean:
	rm -f /p/a/t/h main main.o $(OBJ)/add.o utilities.o debug
