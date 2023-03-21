//Written by: Jared Wilson
//COP 3402 - Assignment 3 Parser Code Generator
//Due: 12/03/2021

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "compiler.h"

#define MAX_CODE_LENGTH 1000
#define MAX_SYMBOL_COUNT 100
#define MAX_SYMBOL_TABLE_SIZE 500

instruction *code;
int codeIndex;
symbol *table;
int tableIndex;

lexeme *temp;
int tempIndex;

int token;
char tokenName[12];
int tokenValue;

int currentLevel;
int symidx;

void program();
void block();
void const_declaration();
int var_declaration();
void procedure_declaration();
int multiple_declarartion_check(char name[]);
int find_symbol(char name[], int kind);
void statement();
void condition();
void expression();
void term();
void factor();
void mark();
void getToken();

void emit(int opname, int level, int mvalue);
void addToSymbolTable(int k, char n[], int v, int l, int a, int m);
void printparseerror(int err_code);
void printsymboltable();
void printassemblycode();

//-s (symbols): printTable
//-a(codes): printCode 
instruction *parse(lexeme *list, int printTable, int printCode)
{
	int i = 0;
	tempIndex = 0;
	symidx = 0;
	tableIndex = 0;
	code = NULL;
	table = NULL;

	// temp = malloc(sizeof(struct lexeme) * 500);

	code = malloc(sizeof(struct instruction) * MAX_CODE_LENGTH);
  	table = malloc(sizeof(struct symbol) * MAX_SYMBOL_TABLE_SIZE);

	temp = list;

	//print lex list
	// while(temp[i].type != -1){
	// 	printf("%s\t\t%d\t\t%d\n", temp[i].name, temp[i].type, temp[i].value);
	// 	i++;
	// }
	getToken();	
	program();
	if(printTable == 1)
		printsymboltable();
	if(printCode == 1)
		printassemblycode();

	//free(temp);
	code[codeIndex].opcode = -1;		/* this line is EXTREMELY IMPORTANT, you MUST uncomment it
											when you test your code otherwise IT WILL SEGFAULT in 
											vm.o THIS LINE IS HOW THE VM KNOWS WHERE THE CODE ENDS
											WHEN COPYING IT TO THE PAS
										*/
	return code;

}

//program ::= block "." . 
void program(){
	int i = 0;
	emit(7, 0, 0);										//emit JMP = 7
	addToSymbolTable(3, "main", 0, 0, 0, 0);			//add to tsymbol table(kind 3, "main", 0, level = 0, 0, unmarked)
	currentLevel = -1;									//current level set to -1
	block();											//BLOCK
	if(token != periodsym)								//if token != period sym
		printparseerror(1);									//error
	emit(9, 0, 3);										//emit HALT (HALT is always 9 0 3)
	while(temp[i].type != -1){							//for each line in code
		if(token == callsym)								//if line has OPR 5 (CALL)
			code[i].m = table[code[i].m].addr;		//given statement
		i++;
	}
	code[0].m = table[0].addr;							//given statement

	
}

//block ::= const-declaration  var-declaration  procedure-declaration statement.	
void block(){
	int procedure_idx;									//procedure index in symbol table
	char name[12];

	currentLevel++;										//increment level
	procedure_idx = tableIndex - 1;						//given statement
	const_declaration(token, name);							//CONST-DECLARATION
	int x = var_declaration(token, name);						//x = VAR-DECLARATION
	procedure_declaration(token, name);							//PROCEDURE-DECLARATION
	table[procedure_idx].addr = codeIndex * 3;			//given statement
	if(currentLevel == 0)								//if level == 0
		emit(6, 0, x);									//emit INC (M = x); INC = 6
	else
		emit(6, 0, x + 3);								//emit INC (M = x + 3)
	
	statement();										//STATEMENT
	mark();												//MARK
	currentLevel--;										//decrement level
}

//const-declaration ::= ["const" ident ":=" number {"," ident ":=" number} ";"].	
void const_declaration(){
	int value;
	char name[12];
	if(token == constsym){							//if token == const
		do{															//do 
			getToken();												//get next token
			if(token != identsym )						//if token != identsym
				printparseerror(2);											//error
			symidx = multiple_declarartion_check(tokenName);				//given statement
			if(symidx != -1)											//if symidx != -1
				printparseerror(18);											//error
			strcpy(name, tokenName);									//save ident name
			value = tokenValue;										//save ident value
			getToken();												//get next token
			if(token != assignsym)						//if token != assignsym
				printparseerror(2);											//error
			getToken();												//get next token
			if(token != numbersym)						//if token != numbersym
				printparseerror(2);											//error
			addToSymbolTable(1, name, value, currentLevel, 0, 0);		//add to symbol table
			getToken();												//get next token
		  }while(token == commasym);					//while token == commasym

		if(token != semicolonsym){					//if token != semicolonsym
			if(token == identsym)						//if token == identsym
				printparseerror(13);										//error
			else													//else
				printparseerror(14);										//error
		}
		getToken();												//get next token
	  }
}

//var-declaration  ::= [ "var "ident {"," ident} “;"].
int var_declaration(){
	int numVars = 0;
	char name[12];
	if(token == varsym){
		do{
			numVars++;
			getToken();
			if(token != identsym)
				printparseerror(3);
			symidx = multiple_declarartion_check(tokenName);
			if(symidx != -1)
				printparseerror(18);
			if(currentLevel == 0){
				addToSymbolTable(2, tokenName, 0, currentLevel, numVars - 1, 0);
			}
			else{
				addToSymbolTable(2, tokenName, 0, currentLevel, numVars + 2, 0);
			}
			getToken();
		}while(token == commasym);

		if(token != semicolonsym){
			if(token == identsym)
				printparseerror(13);
			else
				printparseerror(14);
		}
		getToken();
	}
	return numVars;
}

//procedure-declaration ::= { "procedure" ident ";" block ";" }.
void procedure_declaration(){
	char name[12];
	while(token == procsym){
		getToken();
		if(token != identsym)
			printparseerror(4);
		symidx = multiple_declarartion_check(tokenName);
		if(symidx != -1)
			printparseerror(18);
		addToSymbolTable(3, tokenName, 0, currentLevel, 0, 0);
		getToken();
		if(token != semicolonsym)
			printparseerror(14);
		getToken();
		block();
		if(token != semicolonsym)
			printparseerror(14);
		getToken();
		emit(2, 0, 0);
	}
}

/*	statement   ::= [ ident ":=" expression
 *					| "call" ident  
 *	      			| "begin" statement { ";" statement } "end" 
 *	      			| "if" condition "then" statement ["else" statement]
 *					| "while" condition "do" statement
 *					| "read" ident
 *					| "write" expression
 *	      			| ε ] .  
 */
void statement(){
	int jpcIdx;
	//ident ":=" expression
	if(token == identsym){
		symidx = find_symbol(tokenName, 2);
		if(symidx == -1){
			if(find_symbol(tokenName, 1) != find_symbol(tokenName, 3))
				printparseerror(3);
			else
				printparseerror(19);
		}
		getToken();
		if(token != assignsym)
			printparseerror(5);
		getToken();
		expression();
		emit(4, currentLevel - table[symidx].level, table[symidx].addr);
		return;
	}
	// | "begin" statement { ";" statement } "end"
	if(token == beginsym){
		do{
			getToken();
			statement();
		}while(token == semicolonsym);

		if(token != endsym){
			if(token == identsym)
				printparseerror(15);
			if(token == readsym)
				printparseerror(15);
			if(token == writesym)
				printparseerror(15);
			if(token == beginsym)
				printparseerror(15);
			if(token == callsym)
				printparseerror(15);
			if(token == ifsym)
				printparseerror(15);
			if(token == whilesym)
				printparseerror(15);
			else
				printparseerror(16);
		}
		getToken();
		return;
	}
	// | "if" condition "then" statement ["else" statement]
	if(token == ifsym){
		getToken();
		condition();
		jpcIdx = codeIndex;
		emit(8, 0, 0);	//JPC
		if(token != thensym)
			printparseerror(8);
		getToken();
		statement();
		printf("token: %d\n",token);
		if(token == elsesym){
			printf("inside elsesym\n");
			int jmpIdx = codeIndex;
			emit(7, 0, 0);
			code[jpcIdx].m = codeIndex * 3;
			getToken();
			statement();
			code[jmpIdx].m = codeIndex * 3;
		}
		else
			code[jpcIdx].m = codeIndex * 3;
		return;
	}
	// | "while" condition "do" statement
	if(token == whilesym){
		getToken();
		int loopIdx = codeIndex;
		condition();
		if(token != dosym)
			printparseerror(9);
		getToken();
		jpcIdx = codeIndex;
		emit(8, 0, 0);
		statement();
		emit(7, 0, loopIdx * 3);
		code[jpcIdx].m = codeIndex * 3;
		return;
	}
	// | "read" ident
	if(token == readsym){
		getToken();
		if(token != identsym)
			printparseerror(6);
		symidx = find_symbol(tokenName, 2);
		if(symidx == -1){
			if(find_symbol(tokenName, 1) != find_symbol(tokenName, 3))
				printparseerror(6);
			else
				printparseerror(19);
		}
		getToken();
		emit(9, 0, 2);
		emit(4, currentLevel - table[symidx].level, table[symidx].addr);
		return;
	}
	// | "write" expression
	if(token == writesym){
		getToken();
		expression();
		emit(9, 0, 1);
		return;
	}
	// | "call" ident
	if(token == callsym){
		getToken();
		symidx = find_symbol(tokenName, 3);
		if(symidx == -1){
			if(find_symbol(tokenName, 1) != find_symbol(tokenName, 2))
				printparseerror(7);
			else
				printparseerror(19);
		}
		getToken();
		emit(5, currentLevel - table[symidx].level, symidx);
	}
}

/*
 *	condition ::= "odd" expression
 *				   | expression rel-op expression
 */
void condition(){
	//"odd" expression
	if(token == oddsym){
		getToken();
		expression();
		emit(2, 0 , 6);		//ODD
	}
	// | expression rel-op expression
	// rel-op ::= "=="|“!="|"<"|"<="|">"|">=“.

	else{
		expression();
		if(token == eqlsym){
			getToken();
			expression();
			emit(2, 0, 8);	//EQL
		}
		else if(token = neqsym){
			getToken();
			expression();
			emit(2, 0, 9);	//NEQ
		}
		else if(token == lsssym){
			getToken();
			expression();
			emit(2, 0, 10);		//LSS
		}
		else if(token == leqsym){
			getToken();
			expression();
			emit(2, 0, 11);		//LEQ
		}
		else if(token == gtrsym){
			getToken();
			expression();
			emit(2, 0, 12);		//GTR
		}
		else if(token == geqsym){
			getToken();
			expression();
			emit(2, 0, 13);		//GEQ
		}
		else
			printparseerror(10);
	}
}


//expression ::= [ "+"|"-"] term { ("+"|"-") term}.
void expression(){
	if(token == subsym){
		getToken();
		term();
		emit(2, 0, 1);		//NEG
		while((token == addsym) || (token == subsym)){
			if(token == addsym){
				getToken();
				term();
				emit(2, 0, 2);	//ADD
			}
			else{
				getToken();
				term();
				emit(2, 0, 3);	//SUB
			}			
		}
	}
	else{
		if(token == addsym)
			getToken();
		term();
		while((token == addsym) || (token == subsym)){
			if(token == addsym){
				getToken();
				term();
				emit(2, 0, 2);	//ADD
			}
			else{
				getToken();
				term();
				emit(2, 0, 3);	//SSUB
			}
		}
	}
	if(token == oddsym)
		printparseerror(17);
}


//term ::= factor {("*"|"/"|”%”) factor}. 
void term(){
	factor();
	while((token == multsym) || (token == divsym) || (token == modsym)){
		if(token == multsym){
			getToken();
			factor();
			emit(2, 0, 4);	//MUL
		}
		else if(token == divsym){
			getToken();
			factor();
			emit(2, 0, 5);	//DIV
		}
		else{
			getToken();
			factor();
			emit(2, 0, 7);	//MOD
		}
	}
}

//factor ::= ident | number | "(" expression ")“ .
void factor(){
	// ident
	if(token == identsym){
		int symidx_var = find_symbol(tokenName, 2);
		int symidx_const = find_symbol(tokenName, 1);
		if((symidx_var == -1) && (symidx_const == -1)){
			if((find_symbol(tokenName, 3)) != -1)
				printparseerror(11);
			else
				printparseerror(19);
		}
		if(symidx_var == -1)
			emit(1, 0, table[symidx_const].val);	//LIT
		else if((symidx_const == -1) || (table[symidx_var].level > table[symidx_const].level))
			emit(3, currentLevel - table[symidx_var].level, table[symidx_var].addr);	//LOD
		else
			emit(1, 0, table[symidx_const].val);	//LIT
		getToken();
	}
	// | number
	else if(token == numbersym){
		emit(1, 0, tokenValue);	//LIT
		getToken();
	}
	// | "(" expression ")"
	else if(token == lparensym){
		getToken();
		expression();
		if(token != rparensym)
			printparseerror(12);
		getToken();
	}
	else
		printparseerror(11);
}

/*
 *	does a linear pass through the symbol table looking for symbol name given
 *	if it finds that name, checks to see if its unmarked
 *	if it finds an unmarked instance it checks the level
 *	if its level == current level
 *	return that index
 *	else continue searching
 *	return -1
 */
int multiple_declarartion_check(char name[]){
	for(int i = 0; i < tableIndex; i++){
		if(!strcmp(name, table[i].name))
			if(table[i].mark == 0)
				if(table[i].level == currentLevel)
					return i;
	}
	return -1;
}

/*
 *	does a linear search for the given name
 *	entry matches only if it has the correct name && kind value && is unmarked
 *	then maximize the level value	
 */
int find_symbol(char name[], int kind){
	for(int i = 0; i < tableIndex; i++){
		if(strcmp(name, table[i].name) == 0){
			if(kind == table[i].kind){
				if(table[i].mark == 0){
					//currentLevel = i;
					return i;
				}
			}
		}
	}
	return -1;
}

/*
 *	starts at the end of the table and works backwards
 *	ignores marked entries
 *	if entries level == current level
 *	mark entry
 *	stops when an unmarked entry level < current level
 */
void mark(){
	int i = 0;
	for(i = tableIndex - 1; i >= 0; i--){
		if(table[i].mark == 0){			//only unmarked entries
			if(table[i].level == currentLevel)
				table[i].mark = 1;
			if(table[i].level < currentLevel)
				break;
		}
	}	
}

void getToken(){
	token = temp[tempIndex].type;
	strcpy(tokenName, temp[tempIndex].name);
	tokenValue = temp[tempIndex].value;
	tempIndex++;
}
void emit(int opname, int level, int mvalue)
{
	if(codeIndex > MAX_CODE_LENGTH)
		printparseerror(0);		//code exceeds max length

	code[codeIndex].opcode = opname;
	code[codeIndex].l = level;
	code[codeIndex].m = mvalue;
	codeIndex++;
}

void addToSymbolTable(int k, char n[], int v, int l, int a, int m)
{
	table[tableIndex].kind = k;
	strcpy(table[tableIndex].name, n);
	table[tableIndex].val = v;
	table[tableIndex].level = l;
	table[tableIndex].addr = a;
	table[tableIndex].mark = m;
	tableIndex++;
}




void printsymboltable()
{
	int i;
	printf("Symbol Table:\n");
	printf("Kind | Name        | Value | Level | Address | Mark\n");
	printf("---------------------------------------------------\n");
	for (i = 0; i < tableIndex; i++)
		printf("%4d | %11s | %5d | %5d | %5d | %5d\n", table[i].kind, table[i].name, table[i].val, table[i].level, table[i].addr, table[i].mark); 
	
	free(table);
	table = NULL;
}

void printassemblycode()
{
	int i;
	printf("Line\tOP Code\tOP Name\tL\tM\n");
	for (i = 0; i < codeIndex; i++)
	{
		printf("%d\t", i);
		printf("%d\t", code[i].opcode);
		switch (code[i].opcode)
		{
			case 1:
				printf("LIT\t");
				break;
			case 2:
				switch (code[i].m)
				{
					case 0:
						printf("RTN\t");
						break;
					case 1:
						printf("NEG\t");
						break;
					case 2:
						printf("ADD\t");
						break;
					case 3:
						printf("SUB\t");
						break;
					case 4:
						printf("MUL\t");
						break;
					case 5:
						printf("DIV\t");
						break;
					case 6:
						printf("ODD\t");
						break;
					case 7:
						printf("MOD\t");
						break;
					case 8:
						printf("EQL\t");
						break;
					case 9:
						printf("NEQ\t");
						break;
					case 10:
						printf("LSS\t");
						break;
					case 11:
						printf("LEQ\t");
						break;
					case 12:
						printf("GTR\t");
						break;
					case 13:
						printf("GEQ\t");
						break;
					default:
						printf("err\t");
						break;
				}
				break;
			case 3:
				printf("LOD\t");
				break;
			case 4:
				printf("STO\t");
				break;
			case 5:
				printf("CAL\t");
				break;
			case 6:
				printf("INC\t");
				break;
			case 7:
				printf("JMP\t");
				break;
			case 8:
				printf("JPC\t");
				break;
			case 9:
				switch (code[i].m)
				{
					case 1:
						printf("WRT\t");
						break;
					case 2:
						printf("RED\t");
						break;
					case 3:
						printf("HAL\t");
						break;
					default:
						printf("err\t");
						break;
				}
				break;
			default:
				printf("err\t");
				break;
		}
		printf("%d\t%d\n", code[i].l, code[i].m);
	}
	if (table != NULL)
		free(table);
}

void printparseerror(int err_code)
{
	//printf("error #%d: on token: %d\n", err_code, token);
	switch (err_code)
	{
		case 1:
			printf("Parser Error: Program must be closed by a period\n");
			break;
		case 2:
			printf("Parser Error: Constant declarations should follow the pattern 'ident := number {, ident := number}'\n");
			break;
		case 3:
			printf("Parser Error: Variable declarations should follow the pattern 'ident {, ident}'\n");
			break;
		case 4:
			printf("Parser Error: Procedure declarations should follow the pattern 'ident ;'\n");
			break;
		case 5:
			printf("Parser Error: Variables must be assigned using :=\n");
			break;
		case 6:
			printf("Parser Error: Only variables may be assigned to or read\n");
			break;
		case 7:
			printf("Parser Error: call must be followed by a procedure identifier\n");
			break;
		case 8:
			printf("Parser Error: if must be followed by then\n");
			break;
		case 9:
			printf("Parser Error: while must be followed by do\n");
			break;
		case 10:
			printf("Parser Error: Relational operator missing from condition\n");
			break;
		case 11:
			printf("Parser Error: Arithmetic expressions may only contain arithmetic operators, numbers, parentheses, constants, and variables\n");
			break;
		case 12:
			printf("Parser Error: ( must be followed by )\n");
			break;
		case 13:
			printf("Parser Error: Multiple symbols in variable and constant declarations must be separated by commas\n");
			break;
		case 14:
			printf("Parser Error: Symbol declarations should close with a semicolon\n");
			break;
		case 15:
			printf("Parser Error: Statements within begin-end must be separated by a semicolon\n");
			break;
		case 16:
			printf("Parser Error: begin must be followed by end\n");
			break;
		case 17:
			printf("Parser Error: Bad arithmetic\n");
			break;
		case 18:
			printf("Parser Error: Confliciting symbol declarations\n");
			break;
		case 19:
			printf("Parser Error: Undeclared identifier\n");
			break;
		default:
			printf("Implementation Error: unrecognized error code\n");
			break;
	}
	
	free(code);
	free(table);
	//exit(0);
}