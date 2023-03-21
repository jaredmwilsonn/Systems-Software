/* 
	This is the lex.c file for the UCF Fall 2021 Systems Software Project.
	For HW2, you must implement the function lexeme *lexanalyzer(char *input).
	You may add as many constants, global variables, and support functions
	as you desire.
	
	If you choose to alter the printing functions or delete list or lex_index, 
	you MUST make a note of that in you readme file, otherwise you will lose 
	5 points.
*/

//Written by: Jared Wilson
//COP 3402 - Assignment 2 LEX
//Due: 10/25/2021


#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "compiler.h"
#define MAX_NUMBER_TOKENS 500
#define MAX_IDENT 14
#define MAX_SYM 18
#define MAX_IDENT_LEN 11
#define MAX_NUMBER_LEN 5
#define MAX_SYM_LEN 3

lexeme *list;               //global Lexeme List ptr
int lex_index;          //global integer to track list index

//empty arrays for storage
char identArray[MAX_IDENT_LEN];
char numArray[MAX_NUMBER_LEN];
char symArray[MAX_SYM_LEN];

//arrays for special symbols & words
char specialSym[MAX_SYM][MAX_SYM_LEN] = { ":=", "+", "-", "*", "/", "%", "==", "!=", "<", "<=", ">", ">=", "na", "(", ")", ",", ".", ";" };
char reservedWords[MAX_IDENT][MAX_IDENT_LEN] = {               // given reserved words
                                            "const",    "var",      "procedure",    "begin",    "end",  "while",    "do",   
                                            "if",       "then",     "else",     "call",     "write",    "read",     "odd"
                                        };

//prototype functions
void printlexerror(int type);
void printtokens();
void emptyArray(char *array);
void reservedWordCheck(char *array);
int symbolCheck(char *array);
int findNext(char *input, int i);
void tokenAssigner();


lexeme *lexanalyzer(char *input)
{
    printf("\n\n");     //this line needs to be here otherwise nothing prints on my end, works fine in terminal
    int i = 0;
    int j = 0;
    lex_index = 0;
    list = (struct lexeme *) malloc(sizeof(struct lexeme));

    while(input[i] != '\0'){
        i = findNext(input, i);

        //if the character is a letter
        if(isalpha(input[i])){
            while((isalpha(input[i]) || (isdigit(input[i])))){      //while the character remains a letter or a number
                if(j > 12){      // excessive identifier length error
                    printlexerror(4);
                    return list;
                }
                identArray[j] = input[i];
                j++;
                i++;               
            }

            strcpy(list[lex_index].name, identArray);       //list[lex_index].name = identArray
            reservedWordCheck(identArray);
            emptyArray(identArray);
            j = 0;
            lex_index++;
            i = findNext(input, i);
        }

        //if the character is a number
        if(isdigit(input[i])){
            while(isdigit(input[i])){       //while the character reamins a number
                if(j > 6){               //excessive number error
                    printlexerror(3);
                    return list;
                }
                numArray[j] = input[i];
                j++;
                i++;
                if(isalpha(input[i])){       //invalid identifier error
                    printlexerror(2);
                    return list;
                }
            }
            
            // list[lex_index].value = atoi(numArray);
            strcpy(list[lex_index].name, numArray);         //list[lex_index].name = numArray
            list[lex_index].value = 15;
            emptyArray(numArray);
            j = 0;
            lex_index++;
            i = findNext(input, i);
        }
        // printf("before ispunct: %c\n", input[i]);
        //if the character is as punctuation symbol
        if(ispunct(input[i])){
            // printf("in ispunct: %c\n", input[i]);
            while(ispunct(input[i])){           //while the character reamins a punctuation symbol

                // check for these 5 symbols that have the possibility of also including an '='
                if( (input[i] == ':') || (input[i] == '=') || (input[i] == '!') || (input[i] == '<') || (input[i] == '>')){
                    symArray[j] = input[i];
                    j++;
                    i++;
                    if(input[i] == '=')     //if next symbol is an '=' include it
                        symArray[j] = input[i];
                    break;
                }
                symArray[j] = input[i];
                break;
            }
                i++;
                j++;

                symbolCheck(symArray);
                if(symbolCheck(symArray) == -1){
                    printlexerror(1);
                    return list;
                }
                symbolCheck(symArray);
                emptyArray(symArray);
                j = 0;
        }          
    }
    tokenAssigner();
    printtokens();
	return list;
}

//skips invisible characters and comments
int findNext(char *input, int i)
{
    while((iscntrl(input[i]) || (isspace(input[i]))))       //skips control characters and spaces
        i++;
    if((input[i] == '/') && (input[i++] == '/')){       //skips comments starting at '//'; ending at '\n'
            while(input[i] != '\n')
                i++;
        }
    return i;
}

//matches special symbols and assigns their values
int symbolCheck(char *array)
{
    for(int k = 0; k <MAX_SYM; k++){
        if(strcmp(array, specialSym[k]) == 0){        // compare symbols words
            list[lex_index].value = k + 16;             // assign correct token value
            strcpy(list[lex_index].name, symArray);
            lex_index++;
            return 1; 
        }    
    }
    printf("this broke it: '%s'\n", array);
    return -1;
}

//matches reserved words and assigns their values
void reservedWordCheck(char *array)
{
    for(int k = 0; k < sizeof(reservedWords) - 1; k++){
        if(strcmp(array, reservedWords[k]) == 0){       // compare reserved words
            list[lex_index].value = k + 1;
            return;
        }
    }
    if(strcmp(array, "odd") == 0){
        list[lex_index].value = 28;
        return;
    }
    list[lex_index].value = 14;                         // if no match for reserved words, store as identifier
    //strcpy(list[lex_index].name, "identifiers");
    return;
}


//clears placeholder arrays after each use
void emptyArray(char *array)
{
    for(int i = 0; i < sizeof(array); i++)
    {
        array[i] = '\0';
    }
}

//assigns .type based on .value
void tokenAssigner()
{
    for(int i = 0; i < lex_index; i++){
        if(list[i].value == 1)
            list[i].type = constsym;
        if(list[i].value == 2)
            list[i].type = varsym;
        if(list[i].value == 3)
            list[i].type = procsym;
        if(list[i].value == 4)
            list[i].type = beginsym;
        if(list[i].value == 5)
            list[i].type = endsym;
        if(list[i].value == 6)
            list[i].type = whilesym;
        if(list[i].value == 7)
            list[i].type = dosym;
        if(list[i].value == 8)
            list[i].type = ifsym;
        if(list[i].value == 9)
            list[i].type = thensym;
        if(list[i].value == 10)
            list[i].type = elsesym;
        if(list[i].value == 11)
            list[i].type = callsym;
        if(list[i].value == 12)
            list[i].type = writesym;
        if(list[i].value == 13)
            list[i].type = readsym;
        if(list[i].value == 14)
            list[i].type = identsym;
        if(list[i].value == 15)
            list[i].type = numbersym;
        if(list[i].value == 16)
            list[i].type = assignsym;
        if(list[i].value == 17)
            list[i].type = addsym;
        if(list[i].value == 18)
            list[i].type = subsym;
        if(list[i].value == 19)
            list[i].type = multsym;
        if(list[i].value == 20)
            list[i].type = divsym;
        if(list[i].value == 21)
            list[i].type = modsym;
        if(list[i].value == 22)
            list[i].type = eqlsym;
        if(list[i].value == 23)
            list[i].type = neqsym;
        if(list[i].value == 24)
            list[i].type = lsssym;
        if(list[i].value == 25)
            list[i].type = leqsym;
        if(list[i].value == 26)
            list[i].type = gtrsym;
        if(list[i].value == 27)
            list[i].type = geqsym;
        if(list[i].value == 28)
            list[i].type = oddsym;
        if(list[i].value == 29)
            list[i].type = lparensym;
        if(list[i].value == 30)
            list[i].type = rparensym;
        if(list[i].value == 31)
            list[i].type = commasym;
        if(list[i].value == 32)
            list[i].type = periodsym;
        if(list[i].value == 33)
            list[i].type = semicolonsym;
    }
}

//given print function
void printtokens()
{
	int i;
	printf("Lexeme Table:\n");
	printf("lexeme\t\ttoken type\n");
	for (i = 0; i < lex_index; i++)
	{
		switch (list[i].type)
		{
			case oddsym:
				printf("%11s\t%d", "odd", oddsym);
				break;
			case eqlsym:
				printf("%11s\t%d", "==", eqlsym);
				break;
			case neqsym:
				printf("%11s\t%d", "!=", neqsym);
				break;
			case lsssym:
				printf("%11s\t%d", "<", lsssym);
				break;
			case leqsym:
				printf("%11s\t%d", "<=", leqsym);
				break;
			case gtrsym:
				printf("%11s\t%d", ">", gtrsym);
				break;
			case geqsym:
				printf("%11s\t%d", ">=", geqsym);
				break;
			case modsym:
				printf("%11s\t%d", "%", modsym);
				break;
			case multsym:
				printf("%11s\t%d", "*", multsym);
				break;
			case divsym:
				printf("%11s\t%d", "/", divsym);
				break;
			case addsym:
				printf("%11s\t%d", "+", addsym);
				break;
			case subsym:
				printf("%11s\t%d", "-", subsym);
				break;
			case lparensym:
				printf("%11s\t%d", "(", lparensym);
				break;
			case rparensym:
				printf("%11s\t%d", ")", rparensym);
				break;
			case commasym:
				printf("%11s\t%d", ",", commasym);
				break;
			case periodsym:
				printf("%11s\t%d", ".", periodsym);
				break;
			case semicolonsym:
				printf("%11s\t%d", ";", semicolonsym);
				break;
			case assignsym:
				printf("%11s\t%d", ":=", assignsym);
				break;
			case beginsym:
				printf("%11s\t%d", "begin", beginsym);
				break;
			case endsym:
				printf("%11s\t%d", "end", endsym);
				break;
			case ifsym:
				printf("%11s\t%d", "if", ifsym);
				break;
			case thensym:
				printf("%11s\t%d", "then", thensym);
				break;
			case elsesym:
				printf("%11s\t%d", "else", elsesym);
				break;
			case whilesym:
				printf("%11s\t%d", "while", whilesym);
				break;
			case dosym:
				printf("%11s\t%d", "do", dosym);
				break;
			case callsym:
				printf("%11s\t%d", "call", callsym);
				break;
			case writesym:
				printf("%11s\t%d", "write", writesym);
				break;
			case readsym:
				printf("%11s\t%d", "read", readsym);
				break;
			case constsym:
				printf("%11s\t%d", "const", constsym);
				break;
			case varsym:
				printf("%11s\t%d", "var", varsym);
				break;
			case procsym:
				printf("%11s\t%d", "procedure", procsym);
				break;
			case identsym:
				printf("%11s\t%d", list[i].name, identsym);
				break;
			case numbersym:
				printf("%11s\t%d", list[i].name, numbersym);    //changed to print the name instead of value becasue of how my program worked out
				break;
		}
        printf("\n");
	}
    printf("\n");
	printf("Token List:\n");
	for (i = 0; i < lex_index; i++)
	{
		if (list[i].type == numbersym)
			printf("%d %s ", numbersym, list[i].name);      //changed to print the name instead of value becasue of how my program worked out
		else if (list[i].type == identsym)
			printf("%d %s ", identsym, list[i].name);
		else
			printf("%d ", list[i].type);
	}
	printf("\n");
	list[lex_index++].type = -1;
}

//given error print function
void printlexerror(int type)
{
	if (type == 1)
		printf("Lexical Analyzer Error: Invalid Symbol\n");
	else if (type == 2)
		printf("Lexical Analyzer Error: Invalid Identifier\n");
	else if (type == 3)
		printf("Lexical Analyzer Error: Excessive Number Length\n");
	else if (type == 4)
		printf("Lexical Analyzer Error: Excessive Identifier Length\n");
	else
		printf("Implementation Error: Unrecognized Error Type\n");
	
    list = NULL;    //added
	free(list);
}
