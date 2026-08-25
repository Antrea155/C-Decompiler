This is my C decompiler project, developed as part of my dissertation.
The decompiler that takes x86/x86-64 assembly listings in AT&T syntax and attempts to reconstruct C++ source, supporting constructs such as function calls, while loops and for loops and more.

some basic points regarding work flow of the program

assembly listing (.s)
        |
   parser.c
        |
 function/basic-block representation
        |
     cfg.c
        |
 Control-flow graph
        |
    dflow.c
        |
 Data-flow analysis
        |
    cflow.c
        |
 Control-flow reconstruction
        |
    recover.c
        |
 recovered C/C++ source

assembly.s
 |
 |-- instructions
 |-- labels
 |-- functions
 |-- strings
 |-- operands
        |
   internal Blocks

(I'm using mingw32)

To debug the code:
       -> Clean .o* files
       -> Run : mingw32-make CFLAGS="-Wall -Wextra -std=c11 -g -O0"
       -> gdb ./dec.exe
       -> example : run --mode 64 --graph ../examples/loop.s
       
Compile commands: (Inside src/)

       -> Run : mingw32-make
              -> Regenerate .o files

       -> Run :  mingw32-make clean
              -> To clean the .o files (except .\utils\ll.o) 
       -> Run : Remove-Item .\utils\ll.o
              -> To clean .\utils\ll.o

       -> Run : gcc main.o cfg.o dflow.o cflow.o init.o parser.o utils/ll.o -o dec.exe
              -> this command links all .o files together and creates an executable

       gcc -Wall -Wextra -std=c11 -c main.c -o main.o
       gcc -Wall -Wextra -std=c11 -c cfg.c -o cfg.o
       gcc -Wall -Wextra -std=c11 -c dflow.c -o dflow.o
       gcc -Wall -Wextra -std=c11 -c cflow.c -o cflow.o
       gcc -Wall -Wextra -std=c11 -c init.c -o init.o
       gcc -Wall -Wextra -std=c11 -c parser.c -o parser.o
       gcc -Wall -Wextra -std=c11 -c utils/ll.c -o utils/ll.o

Example on how to run the decompiler : 
-> Run : ./dec.exe --mode 64 --graph ../examples/loop.s

Graph
 -> Download graphviz-15.1.1 (64-bit) EXE installer 
 -> you need a dot folder in your C-Decompiler folder, not in the src code.
 -> (Inside dot/) Run : dot -Tsvg cfgimages.dot -o cfgimages.svg