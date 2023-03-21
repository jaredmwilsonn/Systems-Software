
//Written by: Jared Wilson
//COP 3402 - Assignment 1 VM
//Due: 09/26/2021

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//macros
#define MAX_PAS_LENGTH 500      //Maximum 'process address space' length

//struct
struct instruction{
    int OP;     //operation code
    int L;      //lexicographical level
    int M;      //depending of the operators it indicates:
                    //a number (instructions: LIT, INC)
                    //a prgram address (instructions: JMP, JPC, CAL)
                    //a data address (instructions: LOD, STO)
                    //the identity of the operator OPR (e.g. OPR 0 2 (ADD) or OPR 0 4 (MUL))
};

//global varibales (because of given function: 'int base(int L)')
    int pas[MAX_PAS_LENGTH];          //'process address space' - memory area of given size, 500
    int bp = 0;                       //'base pointer' - points to base of DATA or activation records

//function prototypes
int base(int L);                      //given function
void print_execution(int line, char *opname, struct instruction IR, int PC, int BP, int SP, int DP, int *pas, int GP);      //given function

//main
int main(int argc, char **argv){
    //project address space (pas) & registers
    int input = 0;
    int dp = 0;                       //'data pointer'
    int gp = 0;                       //'global pointer'
    int FREE = 0;                     //'heap pointer'
    int ic = 0;                         //'instruction counter' - points to an instruction in the program area
    int sp = 0;                       //'stack pointer' - points to top of stack
    int pc = 0;                       //'program counter' - fetched from text segment
    int lineNum = 0;                    // pc/3, used to store the line the OP, L, and M came from
    char fileName[150];                 //stores user input
    char *operator;                     //stores text name of executed operation to be printed
    struct instruction ir;              //'instruction register' - fetch cycle: takes instruct from pc; execution cycle: .OP indicates operation to execute
    FILE *fp = NULL;

    //initialize ir
    ir.OP = 0;
    ir.L = 0;
    ir.M = 0;

    //initialize process address space values to zero
    for(int i = 0; i < MAX_PAS_LENGTH; i++){
        pas[i] = 0;
    }

    //printf("\nFile name input: %s\n", fileName);
    // else
    //     fp = fopen("in.txt", "r");
    //D:\\VSCodes\\COP3402\\in.txt

    

    //asks user for fileName, if file not found prompts the user to enter another file name or -1 to exit
    while(1)
    {
        printf("Enter file name: ");
        scanf("%s", fileName);
        fp = fopen(fileName, "r" );
        if(strcmp(fileName, "-1") == 0)
            exit(0);
        //File not found
        if(fp == NULL)
            printf("File not found. Enter -1 to exit or\n");
        else
            break;   
    }

    //scan data from file into pas array
    while(!feof(fp)){
        fscanf(fp, "%d", &pas[ic]);
        ic++;       //counts number of instructions to set the for loop
    }
    fclose(fp);     //close file

    //setup registers (given)
    gp = ic;
    dp = ic - 1;
    FREE = ic + 10;
    bp = ic;
    pc = 0;
    sp = MAX_PAS_LENGTH;

    //print column headers
    printf("\t\t\t\tPC\tBP\tSP\tDP\tdata\n");
    //print initial values
    printf("Initial values:\t\t\t%i\t%i\t%i\t%i\n", pc, bp, sp, dp);

    //fetch and execute cycle
    for(int j = 0; j < ic/3; j++){
        //fetch cycle
        ir.OP = pas[pc];        //sets initial index to operation code
        ir.L = pas[pc+1];       //sets next index to lexicographical level
        ir.M = pas[pc+2];       //sets next index to M - secondary operation code
        lineNum = pc/3;         //stores the line number of the ISA before it is incremented
        pc += 3;

        //execute cycle
        //ISA - 'instruction set architecture'
        switch(ir.OP){
                case 1: //LIT: pushes constant value 'M' onto the stack
                        operator = "LIT";
                        if(bp == gp){
                            dp = dp + 1;
                            pas[dp] = ir.M;
                        }else{
                            sp = sp - 1;
                            pas[sp] = ir.M;
                        }
                        pas[sp] = ir.M;
                        print_execution(lineNum, operator, ir, pc, bp, sp, dp, pas, gp);
                        break;
                case 2: //OPR: operation to be performed on the data at the top of the stack
                        //printf("OPR\n");
                        switch(ir.M){
                                case 0: //RTN: 'register transfer notation'
                                        operator = "RTN";
                                        sp = bp + 1;
                                        bp = pas[sp - 2];
                                        pc = pas[sp - 3];
                                        print_execution(lineNum, operator, ir, pc, bp, sp, dp, pas, gp);
                                        break;
                                case 1: //NEG: negate
                                        operator = "NEG";
                                        if(bp == gp)
                                            pas[dp] = -1*pas[dp];
                                        else
                                            pas[sp] = -1*pas[sp];
                                        print_execution(lineNum, operator, ir, pc, bp, sp, dp, pas, gp);
                                        break;
                                case 2: //ADD: addition
                                        operator = "ADD";
                                        if(bp == gp){
                                            dp = dp - 1;
                                            pas[dp] = pas[dp] + pas[dp+1];
                                        }else{
                                            sp = sp + 1;
                                            pas[sp] = pas[sp] + pas[sp-1];
                                        }
                                        print_execution(lineNum, operator, ir, pc, bp, sp, dp, pas, gp);
                                        break;
                                case 3: //SUB: subtraction
                                        operator = "SUB";
                                        if(bp == gp){
                                            dp = dp - 1;
                                            pas[dp] = pas[dp] - pas[dp+1];
                                        }else{
                                            sp = sp + 1;
                                            pas[sp] = pas[sp] - pas[sp-1];
                                        }
                                        print_execution(lineNum, operator, ir, pc, bp, sp, dp, pas, gp);
                                        break;
                                case 4: //MUL: multiplication
                                        operator = "MUL";
                                        if(bp == gp){
                                            dp = dp - 1;
                                            pas[dp] = pas[dp] * pas[dp+1];
                                        }else{
                                            sp = sp + 1;
                                            pas[sp] = pas[sp] * pas[sp-1];
                                        }
                                        print_execution(lineNum, operator, ir, pc, bp, sp, dp, pas, gp);
                                        break;
                                case 5: //DIV: division
                                        operator = "DIV";
                                        if(bp == gp){
                                            dp = dp - 1;
                                            pas[dp] = pas[dp] / pas[dp+1];
                                        }else{
                                            sp = sp + 1;
                                            pas[sp] = pas[sp] / pas[sp-1];
                                        }
                                        print_execution(lineNum, operator, ir, pc, bp, sp, dp, pas, gp);
                                        break;
                                case 6: //ODD: checks if odd number
                                        operator = "ODD";
                                        if(dp == gp)
                                            pas[dp] = pas[dp] % 2;
                                        else
                                            pas[sp] = pas[sp] % 2;
                                        print_execution(lineNum, operator, ir, pc, bp, sp, dp, pas, gp);
                                        break;
                                case 7: //MOD: modulo
                                        operator = "MOD";
                                        if(bp == gp){
                                            dp = dp - 1;
                                            pas[dp] = pas[dp] % pas[dp+1];
                                        }
                                        else{
                                            sp = sp +1;
                                            pas[sp] = pas[sp] % pas[sp-1];
                                        }
                                        print_execution(lineNum, operator, ir, pc, bp, sp, dp, pas, gp);
                                        break;
                                case 8: //EQL: equal
                                        operator = "EQL";
                                        if(bp == gp){
                                            dp = dp - 1;
                                            pas[dp] = ( pas[dp] == pas[dp+1]);
                                        }else{
                                            sp = sp +1;
                                            pas[sp] = (pas[sp] == pas[sp-1]);
                                        }
                                        print_execution(lineNum, operator, ir, pc, bp, sp, dp, pas, gp);
                                        break;
                                case 9: //NEQ: not equal
                                        operator = "NEQ";
                                        if(bp == gp){
                                            dp = dp - 1;
                                            pas[dp] = (pas[dp] != pas[dp+1]);
                                        }else{
                                            sp = sp +1;
                                            pas[sp] = (pas[sp] != pas[sp-1]);
                                        }
                                        print_execution(lineNum, operator, ir, pc, bp, sp, dp, pas, gp);
                                        break;
                                case 10: //LSS: less than
                                        operator = "LSS";
                                        if(bp == gp){
                                            dp = dp - 1;
                                            pas[dp] = (pas[dp] < pas[dp+1]);
                                        }else{
                                            sp = sp +1;
                                            pas[sp] = (pas[sp] < pas[sp-1]);
                                        }
                                        print_execution(lineNum, operator, ir, pc, bp, sp, dp, pas, gp);
                                        break;
                                case 11: //LEQ: less than or equal
                                        operator = "LEQ";
                                        if(bp == gp){
                                            dp = dp - 1;
                                            pas[dp] = (pas[dp] <= pas[dp+1]);
                                        }else{
                                            sp = sp +1;
                                            pas[sp] = (pas[sp] <= pas[sp-1]);
                                        }
                                        print_execution(lineNum, operator, ir, pc, bp, sp, dp, pas, gp);
                                        break;
                                case 12: //GTR: greater than
                                        operator = "GTR";
                                        if(bp == gp){
                                            dp = dp - 1;
                                            pas[dp] = (pas[dp] > pas[dp+1]);
                                        }else{
                                            sp = sp +1;
                                            pas[sp] = (pas[sp] > pas[sp-1]);
                                        }
                                        print_execution(lineNum, operator, ir, pc, bp, sp, dp, pas, gp);
                                        break;
                                case 13: //GEQ: greater than or equal
                                        operator = "GEQ";
                                        if(bp == gp){
                                            dp = dp - 1;
                                            pas[dp] = (pas[dp] >= pas[dp+1]);
                                        }else{
                                            sp = sp +1;
                                            pas[sp] = (pas[sp] >= pas[sp-1]);
                                        }
                                        print_execution(lineNum, operator, ir, pc, bp, sp, dp, pas, gp);
                                        break;
                                }
                        break;
                case 3: //LOD: load value at top of stack from stack location at offset M from L lexicographical levels down into the data segement
                                //or load value into the data segement
                        operator = "LOD";
                        if(bp == gp){
                            dp = dp + 1;
                            pas[dp] = pas[gp + ir.M];
                        }else{
                            if(base(ir.L) == gp){
                                sp = sp -1;
                                pas[sp] = pas[gp + ir.M];
                            }else{
                                sp = sp -1;
                                pas[sp] = pas[base(ir.L) - ir.M];
                            }
                        }
                        print_execution(lineNum, operator, ir, pc, bp, sp, dp, pas, gp);
                        break;
                case 4: //STO: store value at top of stack in stack location at offset M from L lexicographical levels down into the data segement
                                //or store value into the data segement
                        operator = "STO";
                        if(bp == gp){
                            pas[gp + ir.M] = pas[dp]; 
                            dp = dp - 1;
                        }else{
                            if (base(ir.L) == gp){
                                pas[gp + ir.M] = pas[sp];
                                sp = sp + 1;
                            }else{
                                pas[base(ir.L) - ir.M] = pas[sp];
                                sp = sp + 1;
                                }
                        }
                        print_execution(lineNum, operator, ir, pc, bp, sp, dp, pas, gp);
                        break;
                case 5: //CAL: call procedure at code index M (generates new activation record)
                        operator = "CAL";
                        pas[sp-1] = base(ir.L);        //static link (SL)
                        pas[sp-2] = bp;                         //dynamic link (DL)
                        pas[sp-3] = pc;                         //return address (RA)
                        bp = sp - 1;
                        pc = ir.M;
                        print_execution(lineNum, operator, ir, pc, bp, sp, dp, pas, gp);
                        break;
                case 6: //INC: allocate M memory words
                        operator = "INC";
                        if(bp == gp)
                            dp = dp + ir.M;
                        else
                            sp = sp - ir.M;
                        print_execution(lineNum, operator, ir, pc, bp, sp, dp, pas, gp);
                        break;
                case 7: //JMP: jump to instruction M
                        operator = "JMP";
                        pc = ir.M;
                        print_execution(lineNum, operator, ir, pc, bp, sp, dp, pas, gp);
                        break;
                case 8: //JPC: jump to instruction M if top stack or data element is 0
                        operator = "JPC";
                        if(bp == gp){
                            if(pas[dp] == 0)
                                pc = ir.M;
                            dp = dp -1;
                        }else{
                            if(pas[sp] == 0)
                                pc = ir.M;
                            sp = sp + 1;
                        }
                        print_execution(lineNum, operator, ir, pc, bp, sp, dp, pas, gp);
                        break;
                case 9: //SYS: indicated by M
                        operator = "SYS";
                        switch(ir.M){
                            case 1: //write the top stack element or data element to the screen
                                    if(bp == gp){
                                        printf("Top of Stack Value: %d\n", pas[dp]);
                                        dp = dp -1;
                                    }else{
                                        printf("Top of Stack Value: %d\n", pas[sp]);
                                        sp = sp + 1;
                                    }
                                    
                                    print_execution(lineNum, operator, ir, pc, bp, sp, dp, pas, gp);
                                    break;
                            case 2: //read in input from the user and store it on top of the stack or in data section
                                    printf("Enter an Integer: ");
                                    if(bp == gp){
                                        dp = dp + 1;
                                        scanf("%d", &pas[dp]);
                                    }else{
                                        sp = sp - 1;
                                        scanf("%d", &pas[sp]);
                                    }
                                    print_execution(lineNum, operator, ir, pc, bp, sp, dp, pas, gp);
                                    break;
                            case 3: //end program
                                    print_execution(lineNum, operator, ir, pc, bp, sp, dp, pas, gp);
                                    //printf("----------End of Program----------");
                                    exit(0);
                    }break;
                default:  //default case
                        printf("Default Triggered || Corresponding operation NOT found\n");
                        break;
                }
    }
    return 0;
}

//given base function (Appendix D)
int base(int L){
	int arb = bp;	// arb = 'activation record base'
	while ( L > 0){     //find base L levels down
		arb = pas[arb];
		L--;
	}
	return arb;
}

//given print function
void print_execution(int line, char *opname, struct instruction IR, int PC, int BP, int SP, int DP, int *pas, int GP){
	int i;
	// print out instruction and registers
	printf("%2d\t%s\t%d\t%d\t%d\t%d\t%d\t%d\t", line, opname, IR.L, IR.M, PC, BP, SP, DP);
	
	// print data section
	for (i = GP; i <= DP; i++)
		printf("%d ", pas[i]);
	printf("\n");
	
	// print stack
	printf("\tstack : ");
	for (i = MAX_PAS_LENGTH - 1; i >= SP; i--)
		printf("%d ", pas[i]);
	printf("\n");
}
