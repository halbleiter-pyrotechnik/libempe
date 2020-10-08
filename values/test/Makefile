
OCTAVE = octave
CC = gcc
CFLAGS += -I../ -lm -lc -lgcc
OBJ = tests.elf

all: plot

run: plot
test: plot
plot: $(OBJ)
	./$(OBJ)
	$(OCTAVE) test_all.m

$(OBJ): $(wildcard *.c ../*.c)
	$(CC) $^ $(CFLAGS) -o $@

clean:
	rm -f *.csv *.jpg *.png *.pdf *.o $(OBJ)
