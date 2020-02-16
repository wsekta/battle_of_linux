CC=gcc
CFLAGS=-Wall -Werror
DEPS=

all: Arbiter final

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

final:
	rm -f *.o

Arbiter: main.o
	$(CC) -o Arbiter main.o