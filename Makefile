
#
# Toolchain
#
ARCH        = arm-none-eabi-
CC          = $(ARCH)gcc
AS          = $(ARCH)as
AR          = $(ARCH)ar
LD          = $(ARCH)ld
OBJCOPY     = $(ARCH)objcopy
OBJDUMP     = $(ARCH)objdump
SIZE        = $(ARCH)size
GDB         = $(ARCH)gdb

#
# Compile options
#
LIBS        = -lm -lc -lgcc
INCLUDES    += -I./
OPTIMIZE    = -Og
DEBUG       = -g -Wall
CFLAGS      += $(LIBS) $(INCLUDES) $(OPTIMIZE) $(DEBUG)

#
# Files
#
SRC = $(wildcard */*.c)
OBJ = $(SRC:.c=.o)

#
# Target recipes
#
.PHONY: all clean
all: $(OBJ)

.c.o:
	$(CC) $(CFLAGS) -c $^ -o $@

clean:
	rm -f $(OBJ)
