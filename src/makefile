CC=gcc
CFLAGS=-Wall -Wextra -std=c11

all: dec.exe 

clean:
	del *.o

dec.exe: main.o parser.o cfg.o dflow.o cflow.o init.c utils/ll.o
	$(CC) $(CFLAGS) -o $@ $^


%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@
