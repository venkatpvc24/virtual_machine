PROJDIR = $(realpath $(CURDIR)/.)
SOURCEDIR = $(PROJDIR)/src
INCLUDEDIR = $(PROJDIR)/include
OBJDIR = $(PROJDIR)/obj

dirs:
	@ echo $(PROJDIR)
	@ echo $(SOURCEDIR)
	@ echo $(INCLUDEDIR)

TARGET = vm

VERBOSE = TRUE

CC := gcc 

CFLAGS := -std=c11 -g 

# -Wall -Wextra -Wpedantic  -Wformat=2  \
# -Wno-unused-parameter -Wshadow -Wwrite-strings -Wstrict-prototypes -Wold-style-definition  \
# -Wredundant-decls -Wnested-externs -Wmissing-include-dirs -O2

ifeq ($(VERBOSE), TRUE)
	HIDE = 
else
	HIDE = @
endif

all: $(TARGET)

SOURCES := $(shell find $(SOURCEDIR) -type f -printf '%f\n')

OBJS = $(addprefix $(OBJDIR)/, $(SOURCES:%.c=%.o))

print:
	@echo $(SOURCES)
	
$(TARGET): main.o $(OBJS)
	$(CC) $(CFLAGS) $^ -o $(TARGET)

$(OBJDIR)/%.o: $(SOURCEDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@


clean:
	@ find -name "*.o" -type f -delete
	@ echo "deleted all files with extension '.o'"