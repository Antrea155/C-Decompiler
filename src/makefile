CC=gcc
CFLAGS=-Wall -Wextra -std=c11

all: dec.exe test.exe
		@echo --- Build completed successfully. To run the exetuble, please type "dec" ---

clean:
	del *.o

dec.exe: main.o cfg.o dflow.o cflow.o init.o parser.o utils/ll.o
	$(CC) $(CFLAGS) -o $@ $^

test.exe: testcf.o cfg.o dflow.o cflow.o init.o parser.o utils/ll.o
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@
