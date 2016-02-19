CC=gcc
CFLAGS= -Wall -std=c99
SRC=./game.c
LIB=lib/adventure.c lib/loader.c
HEADER=lib/types.h

all: library
	$(CC) -std=c99 -g $(SRC) -o game -lm -L. -ladventure
library:
	$(CC) -std=c99 -c $(LIB) && \
	ar rc ./libadventure.a ./adventure.o ./loader.o

clean:
	rm -f *.o ; rm -f *.a ; rm -f game
