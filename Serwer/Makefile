CC=gcc
CFLAGS=-Wall -Werror -I -lrt.
DEPS=tools.h timers.h

all: Serwer #final

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

final:
	rm -f *.o

Serwer: main.o tools.o timers.o
	$(CC) -o Serwer main.o tools.o timers.o -lrt