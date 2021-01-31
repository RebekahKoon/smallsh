CC=gcc
CFLAGS=-Wall -std=gnu99

all: smallsh

smallsh: smallsh.o main.o
	$(CC) -g $(CFLAGS) -o smallsh smallsh.o main.o
smallsh.o: smallsh.c smallsh.h
	$(CC) -g $(CFLAGS) -c smallsh.c
main.o: main.c
	$(CC) -g $(CFLAGS) -c main.c

clean:
	-rm *.o

cleanall: clean
	-rm smallsh
