CC=gcc
CFLAGS=-Wall -Werror
DEPS=

all: Hooligan #final

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

final:
	rm -f *.o

Hooligan: main.o
	$(CC) -o Hooligan main.o