CC=gcc
CFLAGS=-Wall -Wextra -pedantic -ansi -std=c99
DEBUG=-g -ggdb -march=native

TARGETS=tema1

build: $(TARGETS)

tema1: tema1.o PlaylistFunctions.o
	$(CC) $(CFLAGS) $(DEBUG) tema1.o PlaylistFunctions.o -o tema1

PlaylistFunctions.o: PlaylistFunctions.h PlaylistFunctions.c
	$(CC) $(CFLAGS) $(DEBUG) PlaylistFunctions.c -c -o PlaylistFunctions.o

tema1.o: tema1.c
	$(CC) $(CFLAGS) $(DEBUG) tema1.c -c -o tema1.o

pack:
	zip -FSr 314CA_TudoracheBogdanMihai_Tema1.zip README Makefile *.c *.h

run: tema1.c
	./tema1 data.in data.out

clean:
	rm -f $(TARGETS)
