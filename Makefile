SRC=game.c
ENTRY=game

all:
	gcc -o $(ENTRY) $(SRC)
debug:
	gcc -g -o $(ENTRY) $(SRC)
