
OCTAVE = octave
CFLAGS += -I../ -lm -lc -lgcc
OBJ = tests.elf

all: plot

plot: $(OBJ)
	./$(OBJ)
	$(OCTAVE) evaluate.m

$(OBJ): $(wildcard *.c ../*.c)
	$(CC) $^ $(CFLAGS) -o $@

clean:
	rm -f *.csv *.o test.elf
