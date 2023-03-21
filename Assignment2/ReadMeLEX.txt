//Written by: Jared Wilson
//COP 3402 - Assignment 2 LEX
//Due: 10/25/2021

This program will accept .txt file containing up to 500 charcters (including spaces).

Adjustements:
driver.c: Added "fclose(ifp)" before the final return.

lex.c: In printtokens() function: 
		for printing list[i].type numbersym: 
		changed print statement to '.name' instead of '.value' because I used .value as a 
		key to assign .type (demonstrated in the function 'tokenAssigner()' ).

	In printlexerror() function:
		Added "list = NULL"; This way list could always be returned from lexanalyzer.

compiler.h: no changes

To compile: "gcc driver.c lex.c -o lex"

To run: "./lex fileName.txt" 

e.x.: 	>gcc driver.c lex.c -o lex
	>./lex test1.txt
