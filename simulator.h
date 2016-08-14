/*
 * Author: Tejeswini
 * Purpose: Header file for 8 stage MIPS integet simulator 
 * Language:  C
 */

/*Constant declared*/

#define BUFFER_SIZE 1000	/*BUFFER_SIZE is declared to create a character input buffer*/
#define NOTUSED -1			/*NOTUSED is a constant declared to indicate the usage of destination register(rd) and 
							second source register(rt) in input instaructions*/
#define MAX_STR 256			/*Constant declared to read the number of characters using fgets. Constant 256 is used as 
							registers are 8 bit registers*/
#define NO_FWD_FLUSHING 1
#define FWD_PREDICT_NOT_TAKEN 2
#define FWD_PREDICT_TAKEN 3


/*Global variables declared*/

int Register[MAX_STR];									/*Declared an integer array of Registers*/
int Memory[MAX_STR];									/*Decalred an integer array for memory locations*/
int count;												/*Integer Variable declared to hold the number of input instructions*/
int inst_counter;										/*Integer variabl declared to hold the current instruction number*/
int inst_cycle;											 /*Integer Variable declared to idicate the cycle count*/
int flush;												/*Integer Variable declared to indicate the occurance of flush*/
int technique;											/*Integer Variable declared to indicate the Branch prediction technique used: 
														1 -> Flushing, 2 -> Predict Not Taken & 3 -> Predict taken*/
int cycle;												/*Integer variable declared to hold processor cycle number*/
char input_buffer[BUFFER_SIZE], temp_buffer[BUFFER_SIZE]; /*Character buffer to hold input*/
struct instruction *inst[30];							/*Array of input instructios of type structure instruction*/



/*File pointers deeclared for input and output file handling*/

FILE *input_file;										/*Declared File pointer for input file to be read*/

FILE *output_file;										/*Declared File Pointer for Output File to be written*/


/*enum having supported opcodes*/

enum type
{
	ADD = 0,						/*ADD opcode for an Addition operation*/
	SUB,							/*SUB opcode for Subtraction operation*/
	LD,								/*LD opcode for Load instruction*/
	SD,								/*SD opcode for store instruction*/
	BNEZ							/*BNEZ opcode for branch instruction*/
};

/*enum having possible states for a pipeline stage*/

enum status
{
	EMPTY,							/*Empty stage for a pipeline*/
	STALL,							/*Stall inserted in pipeline stage due to dependency*/
	ACTIVE							/*Pepeline stage with an active instruction*/
};

/*Structure created for an instruction architecture*/

struct instruction
{
	int rs;							/*Source Register*/
    int rt;							/*Second source register*/
    int rd;							/*Destination register*/
	enum type opcode;				/*Opcode of an instruction*/
    char* label;					/*Instruction label for branching*/
	int offset;						/*offset address*/
	int value;						/*Constant integer value*/
    char target[];					/*Target address for Branch instruction*/
};

/*Structure created for the properties of pipeline stage*/

struct stage
{
	int StageCount;					/*Stage count for a pipeline stage*/
	int instruct_count;				/*Instruction number in a pipeline stage*/
	enum status st;					/*State of pipeline stage: EMPTY, STALL or ACTIVE*/
};

/*The 8 stages of pipeline*/

struct stage *WB;					/*Write-back stage*/
struct stage *MEM3;					/*Memory 3 Stage*/
struct stage *MEM2;					/*Memory 2 Stage*/
struct stage *MEM1;					/*Memory 1 Stage*/
struct stage *EX;					/*Execute Stage*/
struct stage *ID;					/*Instruction Decode Stage*/
struct stage *IF2;					/*Instruction Fetch 2 Stage*/
struct stage *IF1;					/*Instruction Fetch 1 Stage*/
