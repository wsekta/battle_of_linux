CC=gcc
CFLAGS=-Wall -Werror -I -lrt.
DEPS=

all: Klient final

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

final:
	rm -f *.o

Klient: main.o
	$(CC) -o Klient main.o