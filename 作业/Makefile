BIN = 4 4-5 4-6 5-2 5-3

CC = aarch64-linux-gnu-gcc

all: $(BIN)

$(BIN):%:%.c
	$(CC) $^ -o $@ 

clean:
	rm -f *.o $(BIN)
.PHONY: all clean