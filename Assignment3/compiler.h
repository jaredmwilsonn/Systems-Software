/* 
	This is the header file for the UCF Fall 2021 Systems Software Project.
	If you choose to alter this, you MUST make a note of that in your
	readme file, otherwise you will lose 5 points.
*/

typedef enum token_type {
	constsym = 1, varsym, procsym, beginsym, endsym, whilesym, dosym, 
	ifsym, thensym, elsesym, callsym, writesym, readsym, identsym, 
	numbersym, assignsym, addsym, subsym, multsym, divsym, modsym, 
	eqlsym, neqsym, lsssym, leqsym, gtrsym, geqsym, oddsym, 
	lparensym, rparensym, commasym, periodsym, semicolonsym
} token_type;

typedef struct lexeme {
	char name[12];
	int value;
	token_type type;
} lexeme;

typedef struct instruction {
	int opcode;
	int l;              //lexicographical level
	int m;              //modifier
} instruction;

typedef struct symbol {
	int kind;           //const = 1; var = 2; procedure = 3
	char name[12];      //up to 11 characters
	int val;            //ASCII value
	int level;          //L level
	int addr;           //M level
	int mark;           //1 is true 0 is false
} symbol;

lexeme *lexanalyzer(char *input, int printFlag);
instruction *parse(lexeme *list, int printTable, int printCode);
void execute_program(instruction *code, int printFlag);