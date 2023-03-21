//Written by: Jared Wilson
//COP 3402 - Assignment 3 Parser Code Generator
//Due: 12/03/2021

This program requires a .txt file input.

Adjustements:
driver.c: Added "fclose(ifp)" before the final return 0;

parser.c: no relevant changes

compiler.h: no relevant changes

To compile: "gcc driver.c lex.o vm.o parser.c"

To run: "./a.out <fileName.txt> <-command>"

command options:

-l: prints the list and table of lexemes 
-s: prints the symbol table
-a: prints the generated assembly code
-v: prints vm execution trace

commands can be stacked

Example commands:
./a.out input.txt -l -a -v
./a.out input.txt -v
./a.out input.txt -a -s