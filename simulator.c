/*
 * Author: Tejeswini
 * Purpose: To show simulate 8 stage MIPS integer pipeline
 * Language:  C
 */

#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<ctype.h>
#include "simulator.h"


 /*------------------------------------------------- instructionFetch1 -----
         |  Function instructionFetch1
         |
         |  Purpose:  This functions checks for empty, stalland active stats of fetch 2 pipeline stage 
		 |			| and prints to output file 
         |
         |  Parameters:
         |      IF1		IN -- IF1 is a pointer variable to structure of type stage
		 |
         |  Returns:	Void
         *-------------------------------------------------------------------*/

void instructionFetch1(struct stage *IF1)
{
	/*Don't fetch instruction if flush is enabled*/
	if(flush){ return;}

	switch(IF1->st) 
	{
		/*Don't fetch instruction if there is stall*/
		case STALL:							
			break;
		
		/*Fetch instruction if IF1 pipeline stage is empty*/
		case EMPTY:							
		
		/*Fetch instruction if IF1 pipeline stage is active ie., instruction is fetched in previous cycle and moved down the pipeline*/
		case ACTIVE:				

			/*Fetch instruction untill all instructions are fetched*/
			if( (inst_counter ) < count )
				{
					IF1->instruct_count = inst_counter;
					IF1->StageCount = inst_cycle;
					/*set IF1 pipeline tage state to ACTIVE indicating an instruction is fetched in the current cycle*/
					IF1->st = ACTIVE;     
				}
		   else {
					/*Stop fetching after all instructions are fetched*/
					IF1->st = EMPTY;        
				}
			if(IF1->st != EMPTY) 
				{
					/*If IF1 stage is not empty print to output file */
					fprintf( output_file, "I%d-IF1 ", IF1->StageCount );
				}
			inst_counter++;
	}
}

/*------------------------------------------------- instructionFetch2 -----
         |  Function instructionFetch2
         |
         |  Purpose:	This functions checks for empty, stalland active stats of fetch 2 pipeline stage 
					|	and prints to output file 
         |
         |  Parameters:
         |      IF1		IN -- IF2 is a pointer variable to structure of type stage
         |  Returns:	Void
         *-------------------------------------------------------------------*/

void instructionFetch2(struct stage *IF2)
{
	switch(IF2->st) 
	{
		/*Don't fetch Inst if empty*/
		case EMPTY:
			break;
		case STALL:
			fprintf(output_file, "I%d-stall ", IF2->StageCount );
			break;
		case ACTIVE:
			fprintf( output_file, "I%d-IF2 ", IF2->StageCount );
	}
}

/*------------------------------------------------- instructionDecode -----
         |  Function instructionDecode
         |
         |  Purpose:  This function decodes the instruction. It checks for stall, 
		 |	empty and active states of decode stageand prints to the output file. 
		 |	decode stage never halts if forwarding is present. If there is no forwarding, 
		 |	data dependency is checked at decode stage and halt the pipeline. Also, if predict 
		 |	taken technique is used, branch instructions target address is compared with the label 
		 |  of all instructions and correct target instruction is fetched in the next cycle.
         |
         |  Parameters:
         |      IF1		 (IN -- First Pipeline stage that simulates instruction 
		 |						fetching when the pipeline stage is either empty 
		 |						or active. also, no action on stall.
         |  Returns:	Void
         *-------------------------------------------------------------------*/

void instructionDecode(struct stage *ID)
{
	int i;
	switch(ID->st) 
	{
		/*Do nothing if empty*/
		case EMPTY:
			break;
		
		/*If No forwarding hardware and pipeline is stall at decode stage, check for the exisstence of stall*/
		case STALL:
			
			/*Stall at deecode when there is no forwarding hardware*/
			if (technique == NO_FWD_FLUSHING) 
			{
				if(MEM1->st != EMPTY) 
				{
					/*If data dependency is found between instruction at decode stage and 
					instruction at execute stage, stall at decode*/

					if ( ((inst[EX->instruct_count]->rd == inst[ID->instruct_count]->rs)|| 
						(inst[EX->instruct_count]->rd == inst[ID->instruct_count]->rt))  &&  
						(inst[EX->instruct_count]->opcode != SD))
						{
							fprintf( output_file, "I%d-stall ", ID->StageCount );
							return;
						}

					/*If data dependency is found between instruction at decode stage and 
					instruction at Memory1 stage, stall at decode*/

					if( ((inst[MEM1->instruct_count]->rd == inst[ID->instruct_count]->rs) || 
						(inst[MEM1->instruct_count]->rd == inst[ID->instruct_count]->rt)))
						{
							fprintf( output_file, "I%d-stall ", ID->StageCount );
							return;
						}

					/*If data dependency is found between instruction at decode stage and 
					instruction at Memory2 stage, stall at decode*/

					if ( ((inst[MEM2->instruct_count]->rd == inst[ID->instruct_count]->rs)|| 
						(inst[MEM2->instruct_count]->rd == inst[ID->instruct_count]->rt)) )
						{
							fprintf( output_file, "I%d-stall ", EX->StageCount );
							return;
						}
					
					/*If data dependency is found between instruction at decode stage and 
					instruction at Memory3 stage, stall at decode*/

					if ( ((inst[MEM3->instruct_count]->rd == inst[ID->instruct_count]->rs)|| 
						(inst[MEM3->instruct_count]->rd == inst[ID->instruct_count]->rt)) )
						{
							fprintf( output_file, "I%d-stall ", ID->StageCount );
							return;
						}
				}
		
				if(WB->st != EMPTY) 
				{
					/*Data dependency is resolved at writeback stage and stalls are removed*/

					if ( ((inst[WB->instruct_count]->rd == inst[ID->instruct_count]->rs)|| 
						(inst[WB->instruct_count]->rd == inst[ID->instruct_count]->rt)) )
						{
							fprintf( output_file, "I%d-ID ", ID->StageCount );
							IF1->st = ACTIVE;
							IF2->st = ACTIVE;
							ID->st = ACTIVE;
							return;
						}
				}
			}
			/*print decode stage stall to output file*/
			fprintf(output_file, "I%d-stall ", ID->StageCount );
			break;

		/*Check for presence of data dependencies and stall*/
		case ACTIVE:

			/*Stall at decode when there is no forwarding*/
			if (technique == NO_FWD_FLUSHING)
			{
				/*If data dependency is found between instruction at decode stage and 
					instruction at execute stage, stall at decode*/

				if(EX->st != EMPTY) 
				{
					if ( ((inst[EX->instruct_count]->rd == inst[ID->instruct_count]->rs)|| 
						(inst[EX->instruct_count]->rd == inst[ID->instruct_count]->rt))  &&  
						(inst[EX->instruct_count]->opcode != SD))
						{
							fprintf( output_file, "I%d-stall ", ID->StageCount );
							ID->st = IF2->st = IF1->st = STALL;
							MEM1->st = EMPTY;
							return;
						}
				}

				/*If data dependency is found between instruction at decode stage and 
					instruction at Memory1 stage, stall at decode*/

				else if(MEM1->st != EMPTY) 
				{
					if( ((inst[MEM1->instruct_count]->rd == inst[ID->instruct_count]->rs) || 
						(inst[MEM1->instruct_count]->rd == inst[ID->instruct_count]->rt)))
					{
						fprintf( output_file, "I%d-stall ", ID->StageCount );
						ID->st = IF2->st = IF1->st = STALL;
						return;			
					}
				}

				/*If data dependency is found between instruction at decode stage and 
					instruction at Memory2 stage, stall at decode*/

				else if(MEM2->st != EMPTY) 
				{
					if(((inst[MEM2->instruct_count]->rd == inst[ID->instruct_count]->rs) ||
						(inst[MEM2->instruct_count]->rd == inst[ID->instruct_count]->rt)))
					{
						fprintf( output_file, "I%d-stall ", ID->StageCount );
						ID->st = IF2->st = IF1->st = STALL;
						MEM1->st = EMPTY;
						return;
					}
				}

				/*If data dependency is found between instruction at decode stage and 
					instruction at Memory3 stage, stall at decode*/

				else if(MEM3->st != EMPTY) 
				{
					if(((inst[MEM3->instruct_count]->rd == inst[ID->instruct_count]->rs)
						|| (inst[MEM3->instruct_count]->rd == inst[ID->instruct_count]->rt)))
					{
						fprintf( output_file, "I%d-stall ", ID->StageCount );
						ID->st = IF2->st = IF1->st = STALL;
						MEM2->st =  MEM1->st = EMPTY;
						return;
					}
				}
			}

		/*If branch predict taken, compare target address with label of all other instrctions. 
		Flush the pipeline and set the target instruction to fetch*/

		if(technique == FWD_PREDICT_TAKEN) 
		{
			if(inst[ID->instruct_count]->opcode ==  BNEZ)
			{
				for( i = 0 ; i < count ; i++)
				{
					if((inst[i]->label) != NULL) 
					{
						if( strcmp( inst[i]->label, inst[ID->instruct_count]->target) == 0)
						{
							break;
						}					
					}					
				}
				inst_counter = i;
				flush = 1;
			}
		}

		/*Print decode stage to output file*/
		fprintf( output_file, "I%d-ID ", ID->StageCount );
	}
}


/*------------------------------------------------- execute -----
         |  Function	execute
         |
         |  Purpose:	This function processes instructions based on opcode in the structure instruction. 
		 |			    It checks for state of pipeline and checks for data dependencies. If found any data 
		 |				dependency, stalls the pipeline. Also, if predict not taken or flushing technique are used,
		 |				branch instructions target address is compared with the label 
		 |				of all instructions and correct target instruction is fetched in the next cycle.
         |
         |  Parameters:
         |      IF1		IN -- EX is a pointer variabe to structure of type strut
         |  Returns:	Void
         *-------------------------------------------------------------------*/


void execute(struct stage *EX)
{
	int i = 0;
	switch(EX->st) 
	{
		/*Do nothing if the stage is empty*/
		case EMPTY:
			return;
			break;
		
		/*Execute stage of pipeline is stalled in previous cycle, check for exixtence of stall*/
		case STALL:
			/*In the presence of forwarding hardware and data dependency, pipeline is stalled at execute stage*/
			if((technique == FWD_PREDICT_NOT_TAKEN) || (technique == FWD_PREDICT_TAKEN))
			{
				if(MEM1->st != EMPTY) 
				{
					/*If data dependency is found between instruction at execute stage and 
					instruction at Memory1 stage, stall at execute stage*/

					if((inst[MEM1->instruct_count]->opcode == LD) && 
						((inst[MEM1->instruct_count]->rd == inst[EX->instruct_count]->rs) 
						|| (inst[MEM1->instruct_count]->rd == inst[EX->instruct_count]->rt)))
					{
						fprintf( output_file, "I%d-stall ", EX->StageCount );
						return;
					}

					/*If data dependency is found between instruction at execute stage and 
					instruction at Memory2 stage, stall at execute stage*/

					if ((inst[MEM2->instruct_count]->opcode == LD) && 
						((inst[MEM2->instruct_count]->rd == inst[EX->instruct_count]->rs)||
						(inst[MEM2->instruct_count]->rd == inst[inst_counter]->rt)) )
					{
						fprintf( output_file, "I%d-stall ", EX->StageCount );
						return;
					}
				}
				else 
				{
					/*If dependency is no more existing, remove stalls in pipeline*/

					IF1->st = ACTIVE;
					IF2->st = ACTIVE;
					ID->st = ACTIVE;
					EX->st= ACTIVE;
				}	
			}

	case ACTIVE:
		if((technique == FWD_PREDICT_NOT_TAKEN) || (technique == FWD_PREDICT_TAKEN)) 
		{
			/*If data dependency is found between instruction at execute stage and 
					instruction at Memory1 stage, stall at execute stage*/

			if(MEM1->st != EMPTY) 
			{
				if((inst[MEM1->instruct_count]->opcode == LD) && 
					((inst[MEM1->instruct_count]->rd == inst[EX->instruct_count]->rs) || 
					(inst[MEM1->instruct_count]->rd == inst[EX->instruct_count]->rt)))
					{
						fprintf( output_file, "I%d-stall ", EX->StageCount );
						EX->st = ID->st = IF2->st = IF1->st = STALL;
						return;			
					}
			}

			/*If data dependency is found between instruction at execute stage and 
					instruction at Memory2 stage, stall at execute stage*/

			else if(MEM2->st != EMPTY) 
			{
				if((inst[MEM2->instruct_count]->opcode == LD) && 
					((inst[MEM2->instruct_count]->rd == inst[EX->instruct_count]->rs) || 
					(inst[MEM2->instruct_count]->rd == inst[EX->instruct_count]->rt)))
					{
						fprintf( output_file, "I%d-stall ", EX->StageCount );
						EX->st = ID->st = IF2->st = IF1->st = STALL;
						MEM1->st =EMPTY;
						return;
					}
			}
		}
	
		/*Addition operation done at execute stage*/
		if(inst[EX->instruct_count]->opcode == ADD)
		{
			/*Addition operation in immediate addressing mode*/
			if(inst[EX->instruct_count]->rt == NOTUSED)
			{
				Register[((inst[EX->instruct_count]->rd))] = Register[((inst[EX->instruct_count]->rs))] + inst[EX->instruct_count]->value;
			} 
			else /*Addition operation in Register addressing mode*/ 
			{
				Register[((inst[EX->instruct_count]->rd))] = Register[((inst[EX->instruct_count]->rs))] + Register[((inst[EX->instruct_count]->rt))];
			}
		}

		/*Subtraction operation done at execute stage*/
		else if(inst[EX->instruct_count]->opcode == SUB)
		{
			/*Subtraction operation in immediate addressing mode*/
			if(inst[EX->instruct_count]->rt == NOTUSED)
			{
				Register[((inst[EX->instruct_count]->rd))] = Register[((inst[EX->instruct_count]->rs))] - inst[EX->instruct_count]->value;
			} 
			else /*Subtraction operation in Register addressing mode*/
			{
				Register[((inst[EX->instruct_count]->rd))] = Register[((inst[EX->instruct_count]->rs))] - Register[((inst[EX->instruct_count]->rt))];
			}
		}
			
		/*If branch predict not taken or flushing are used, compare target address with label of all other instrctions. 
		Flush the pipeline and set the target instruction to fetch*/

		if((technique == FWD_PREDICT_NOT_TAKEN) || (technique == NO_FWD_FLUSHING))
		{
			if(inst[EX->instruct_count]->opcode ==  BNEZ)
			{
				for( i = 0 ; i < count ; i++)
				{
					if((inst[i]->label) != NULL) 
					{
						/*Compare target of branch instrution with label of all instructions*/
						if( strcmp( inst[i]->label, inst[EX->instruct_count]->target) == 0)
						{
							break;
						}					
					}					
				}
				inst_counter = i;
				flush = 1;
			}
		}
		fprintf( output_file, "I%d-EX ", EX->StageCount );
	}
}


/*------------------------------------------------- memoryStage1 -----
         |  Function memoryStage1
         |
         |  Purpose:  This function handleas empty, stall and active states for MEM1 pipeline stage.
         |
         |  Parameters:
         |      IF1		 IN -- mem1 -> pointer variable to structure of type stage.
         |  
		 |	Returns:	Void
         *-------------------------------------------------------------------*/


void memoryStage1(struct stage *MEM1)
{
	switch(MEM1->st) 
	{
		/*Do nothing if memory1 stage is empty*/
		case EMPTY:
			break;
		case STALL:
			/*print stall to output file*/
			fprintf(output_file, "I%d-stall ", MEM1->StageCount );
			break;
		case ACTIVE:
			/*print stage count and instruction number to output file*/
			fprintf( output_file, "I%d-MEM1 ", MEM1->StageCount );
	}
}


/*------------------------------------------------- memoryStage2 -----
         |  Function	memoryStage2
         |
         |  Purpose:	This function handles the empty, stall and active states of memory 2 stage
		 |				and prints the instruction in memory 2  pipeline stage. Load and store 
		 |				instructions are processed in this stage. Offset is added to memory address 
		 |              and the content of resulted memory location is placed in the register in a 
		 |				load instruction. Where as, content of register is placed in the resulted 
		 |				memory address incase of store instruction.
         |
         |  Parameters:
         |      IF1		IN -- MEM2-> pointer variable to structure of type stage
		 |
         |  Returns:	Void
         *-------------------------------------------------------------------*/

void memoryStage2(struct stage *MEM2)
{
	switch(MEM2->st) 
	{
		/*Do nothing if memory2 stage is empty*/
		case EMPTY:
			break;
		case STALL:
			/*print stall to output file*/
			fprintf(output_file, "I%d-stall ", MEM2->StageCount );
			break;
		case ACTIVE:
			if (inst_counter < count) 
			{
				/*Perform load operation at memory stage 2 and support offset addressing mode*/
				if(inst[MEM2->instruct_count]->opcode == LD)
				{
					Register[((inst[MEM2->instruct_count]->rd))] = Memory[ ( Register[((inst[MEM2->instruct_count]->offset + 
						inst[MEM2->instruct_count]->rs))]) / 8];
				}

				/*Perform store operation at memory stage 2 and support offset addressing mode*/
				else if(inst[MEM2->instruct_count]->opcode == SD)
				{
				Memory[(Register[((inst[MEM2->instruct_count]->offset + inst[MEM2->instruct_count]->rs))]) / 8]= 
					Register[((inst[MEM2->instruct_count]->rt))];
				}
			}
			/*Print memory2 stage to output file*/
			fprintf( output_file, "I%d-MEM2 ", MEM2->StageCount );
	}
}


/*------------------------------------------------- memoryStage3 -----
         |  Function memoryStage3
         |
         |  Purpose:	This function handles the empty, stall and active states of memory 3 stage
		 |				and prints the instruction in memory 3  pipeline stage.
         |  Parameters:
         |      MEM3	IN -- pointer to the structure stage
		 |
         |  Returns:	Void
         *-------------------------------------------------------------------*/

void memoryStage3(struct stage *MEM3)
{
	switch(MEM3->st) 
	{
		case EMPTY:
			/*Do nothing if memory3 stage is empty*/
			break;

		case STALL:
			/*print stall to output file at Memory3 stage*/
			fprintf(output_file, "I%d-stall ", MEM3->StageCount );
			break;
		case ACTIVE:
			/*Print memory3 stage to output file*/
			fprintf( output_file, "I%d-MEM3 ", MEM3->StageCount );
	}
}

/*------------------------------------------------- writeBack -----
         |  Function writeBack
         |
         |  Purpose:  This function handles the empty, stall and active states of write-back stage
		 |            and prints the instruction in the write-back piupeline stage.
         |
         |  Parameters:
         |      WB		 (IN -- pointer to structure stage
         | 
		 |Returns:	Void
         *-------------------------------------------------------------------*/

void writeBack( struct stage *WB)
{	
	switch (WB->st) 
	{
		case EMPTY:
			/*Do nothing if write-back stage is empty*/
			break;
		case STALL:
			/*print stall to output file at Write-back stage*/
			fprintf( output_file, "I%d-stall ", WB->StageCount ); 
			break;
		case ACTIVE:
			/*Print Write-back stage to output file*/
			fprintf( output_file, "I%d-WB ", WB->StageCount);
	}
}

/*------------------------------------------------- init_pipeline -----
         |  Function init_pipeline
         |
         |  Purpose:  This function initializes the pipeline. Memory is allocated for each stage
		 |  with the size of structure stage.  where stage has stage count, instruction count and 
		 |  state of pipeline stage as members of structure. Initially, set the state of each pipeline 
		 |  stage to empty and stage count to 0.
         |      
         |  Returns:	Void
         *-------------------------------------------------------------------*/

void init_pipeline() 
{ 
	/*Create memory for each stage. Each stage is a structure of type stage*/
	IF1 = (struct stage*)malloc(sizeof(struct stage));
	IF2 = (struct stage*)malloc(sizeof(struct stage));
	ID = (struct stage*)malloc(sizeof(struct stage));
	EX = (struct stage*)malloc(sizeof(struct stage));
	MEM1 = (struct stage*)malloc(sizeof(struct stage));
	MEM2 = (struct stage*)malloc(sizeof(struct stage));
	MEM3 = (struct stage*)malloc(sizeof(struct stage));
	WB = (struct stage*)malloc(sizeof(struct stage));

	/*Initialize pipeline stages*/
	IF1->st = EMPTY;
	IF1->StageCount = 1;
	IF2->st = EMPTY;
	IF2->StageCount = 0;
	ID->st = EMPTY;
	ID->StageCount = 0;
	EX->st = EMPTY;
	EX->StageCount = 0;
	MEM1->st = EMPTY;
	MEM1->StageCount = 0;
	MEM2->st = EMPTY;
	MEM2->StageCount =0;
	MEM3->st = EMPTY;
	MEM3->StageCount= 0;
	WB->st = EMPTY;
	WB->StageCount = 0;
	flush = 0;				/*Deselect flush by default*/
	count = 0;				/*Global variable for total number of instructions in input reset*/
	inst_counter=0;         /*Global variable for current instruction set to zero */
	inst_cycle = 1;			/*Cycle count*/
	cycle = 1;
}

/*------------------------------------------------- readInput -----
         |  Function readInput
         |
         |  Purpose:  This function reads input file into a character buffer. 
		 |  From character buffer read the register, memory and code sections seperately 
		 |  and update Register[] array, Memory[] array and inst[] arrays. Reister and 
		 |  Memory are integer arrays and inst[] is an array of structures of type instruction 
		 |  having source register, destination register, opcode, offset, value
		 |  and target address as members
         |
         |  Parameters:
         |      IF1		 IN -- input_file, output_file
		 |		input_file -> file pointer to input file
		 |		output_file -> file pointer to output file
		 |
         |  Returns:	Void
         *-------------------------------------------------------------------*/

void readInput(FILE *input_file, FILE *output_file ) 
{
	const char s[2] = ":";
	int memloc = 0;				/*Integer variable declared to hold the memory address*/
	int memval = 0;				/*Integer variable declared to hold content in memory location*/
	int reg;					/*Register number*/
	int val;					/*Register Content*/
	int regs = 0;				/*Return of sscanf reading input*/
	int res = 0;				/*Result of sscanf*/
	int code = 0;				/*Integer variable declared to validate memory section of input*/
	char *token;
	char *token1;

	fgets(input_buffer, BUFFER_SIZE, input_file);
	if(input_buffer[0] != '\n')
	{
		if(strncmp(input_buffer, "REGISTERS", 9) == 0)
		{
			//Do Nothing
		}
	}

	while(fgets(input_buffer, BUFFER_SIZE, input_file)!= NULL)
	{
		if(input_buffer[0] != '\n')
		{
			if(strncmp(input_buffer, "MEMORY", 6) == 0) break;
			
			/*Read input register addresses and content of registers*/
			regs = sscanf(input_buffer, "R%d %d", &reg, &val) ;
			if (regs != 2)
			{
				puts("\n Input invalid!");
				fprintf(output_file, "Input Invalid");
				exit(0);
			}
			Register[reg] = val;
		}
	}

	while(fgets(input_buffer, BUFFER_SIZE, input_file)!= NULL)
	{
		if(input_buffer[0] != '\n')
		{
			/*Read input Memory addresses and content of memory*/
			if(strncmp(input_buffer, "CODE", 4) == 0) break;
			code  = sscanf(input_buffer, "%d %d", &memloc, &memval) ;
			if (code != 2)
			{
				puts("\n Input invalid!");
				fprintf(output_file, "Input Invalid");
				exit(0);
			}
			Memory[(memloc) / 8] = memval;
		}
	}

	while(fgets(input_buffer, BUFFER_SIZE, input_file) != NULL)
	{
		if(input_buffer[0] != '\n')
		{
			inst[count] = (struct instruction*) malloc( sizeof(struct instruction));	
			if(strstr(input_buffer, ":") != NULL)
				{
					/*Read input Memory label*/
					strcpy(temp_buffer, input_buffer);
					token1 = strtok(temp_buffer, s);
					inst[count]->label = token1;
					token = strtok(NULL, s);
					do { token++; } while( isspace( *token ) );
					strcpy(input_buffer, token);
				} 
			else 
			{
				inst[count]->label = NULL;
			}
			if(strstr(input_buffer, "ADD") != NULL)
			{
				res = sscanf(input_buffer, "%*s R%d, R%d, R%d\n", &inst[count]->rd, &inst[count]->rs, &inst[count]->rt);
				if(res == 3)
				{
					inst[count]->opcode = ADD; /*update opcode field in instruction structure*/
					inst[count]->offset = NOTUSED;	/*State offset is not used in instruction*/
				} 
				else 
				{
					res = sscanf(input_buffer, "%*s R%d, R%d, #%d\n", &inst[count]->rd, &inst[count]->rs, &inst[count]->value);
					inst[count]->opcode = ADD;   /*update opcode field in instruction structure*/
					inst[count]->offset = NOTUSED; /*Set offset not used in instruction*/
					inst[count]->rt = NOTUSED;
				}
			}

			else if(strstr(input_buffer, "SUB") != NULL)
			{
				res = sscanf(input_buffer, "%*s R%d, R%d, R%d\n", &inst[count]->rd, &inst[count]->rs, &inst[count]->rt);
				if(res == 3)
				{
					inst[count]->opcode = SUB;	/*update opcode field in instruction structure*/
					inst[count]->offset = NOTUSED; /*Set offset not used in instruction*/
				} 
				else 
				{
					res = sscanf(input_buffer, "%*s R%d, R%d, #%d\n", &inst[count]->rd, &inst[count]->rs, &inst[count]->value);
					inst[count]->opcode = SUB;	/*update opcode field in instruction structure*/
					inst[count]->offset = NOTUSED;	/*Set offset not used in instruction*/
				}
			}

			else if(strstr(input_buffer, "LD") != NULL)
			{
				sscanf(input_buffer, "%*s R%d, %d(R%d)\n", &inst[count]->rd, &inst[count]->offset, &inst[count]->rs );
				inst[count]->opcode = LD;	/*update opcode field in instruction structure*/
			}	

			else if(strstr(input_buffer, "SD") != NULL)
			{
				sscanf(input_buffer, "%*s R%d, %d(R%d)\n",  &inst[count]->rt, &inst[count]->offset, &inst[count]->rs );
				inst[count]->opcode = SD;	/*update opcode field in instruction structure*/
			}

			else if(strstr(input_buffer, "BNEZ") != NULL)
			{
				sscanf( input_buffer, "%*s R%d, %s\n", &inst[count]->rs, inst[count]->target ) ;
				inst[count]->opcode = BNEZ;	/*update opcode field in instruction structure*/
			}
			count++;
		}
	}
}

/*------------------------------------------------- writeOutput -----
         |  Function writeOutput
         |
         |  Purpose:			Begins writing output to the file as c# follwed by cycle number
         |      
         |  Parameters:
         |      output_file		IN -- file pointer to output file. 
		 |						
         |  Returns:			Void
         *-------------------------------------------------------------------*/

void writeOutput(FILE *output_file) 
{
	/*Start writing to output file*/
	fprintf(output_file, "\n");
	fprintf( output_file, "c#%d ", cycle );
}

/*------------------------------------------------- incrementCycle -----
         |  Function incrementCycle
         |
         |  Purpose:   This function increments processor cycle number and 
		 |			   moves the instruction down the pipeline. 
		 |			   If Instruction fetch1 is not stall, move the instruction from fetch1 to fetch2 stage. 
		 |			   If Instruction fetch2 is not stall, move the instruction from fetch2 to decode stage.
		 |			   If decode is not stall, move the instruction from decode to execute stage.
		 |			   If execute is not stall, move the instruction from execute to memory1 stage.
		 |			   If memory1 is not stall, move the instruction from memory1 to memory2 stage.
		 |			   If memory2 is not stall, move the instruction from memory2 to memory3 stage.
		 |			   If memory3 is not stall, move the instruction from mem3 to write-back stage.
		 |			   Also handles flushing by emptying 
		 |			   IF1, IF2 and ID stages. 
         |
         |  Returns:	Void
         *-------------------------------------------------------------------*/

void incrementCycle() 
{
	/*If memory3 is not stall, move the instruction from mem3 to write-back stage*/
	if(( MEM3->st == ACTIVE) ||( MEM3->st == EMPTY))
	{	
		WB->st = MEM3->st; 
		WB->StageCount = MEM3->StageCount; 
		WB->instruct_count = MEM3->instruct_count; 
	}

	/*If memory2 is not stall, move the instruction from memory2 to memory3 stage*/
	if( MEM2->st != STALL ) 
	{ 
		MEM3->st = MEM2->st; 
		MEM3->StageCount = MEM2->StageCount; 
		MEM3->instruct_count = MEM2->instruct_count; 
	}

	/*If memory1 is not stall, move the instruction from memory1 to memory2 stage*/
	if( MEM1->st != STALL) 
	{ 
		MEM2->st = MEM1->st; 
		MEM2->StageCount = MEM1->StageCount; 
		MEM2->instruct_count = MEM1->instruct_count; 
	} 

	else MEM2->st = EMPTY;
								
	/*If execute is not stall, move the instruction from execute to memory1 stage*/
	if( EX->st != STALL  )
	{ 
		MEM1->st = EX->st; 
		MEM1->StageCount = EX->StageCount; 
		MEM1->instruct_count = EX->instruct_count; 
	}

	if(technique == 1) /*Applied only in the absence of forwarding since pipeline never stalls 
					   at decode stage with forwarding*/
	{
		/*If decode is not stall, move the instruction from decode to execute stage*/
		if( ID->st != STALL  ) 
		{ 
			EX->st = ID->st; 
			EX->StageCount = ID->StageCount; 
			EX->instruct_count = ID->instruct_count; 
		} 

		else EX->st = EMPTY; /*If pipeline is stalled t decode stage without forwarding, make the execute stage empty*/
	}

	else 
	{
		/*If decode is not stall, move the instruction from decode to execute stage*/
		if( ID->st != STALL  ) 
		{ 
			EX->st = ID->st; 
			EX->StageCount = ID->StageCount; 
			EX->instruct_count = ID->instruct_count; 
		} 
		/*If pipeline is stalled at decode stage, leave the memory1 stage empty*/
		else MEM1->st = EMPTY;
	}

	/*If Instruction fetch2 is not stall, move the instruction from fetch2 to decode stage*/
	if( IF2->st != STALL ) 
	{ 
		ID->st = IF2->st; 
		ID->StageCount = IF2->StageCount; 
		ID->instruct_count = IF2->instruct_count; 
	}

	/*If Instruction fetch1 is not stall, move the instruction from fetch1 to fetch2 stage*/
	if( IF1->st != STALL  ) 
	{
		IF2->st = IF1->st; 
		IF2->StageCount = IF1->StageCount; 
		IF2->instruct_count = IF1->instruct_count; 
		++inst_cycle; 
	}

	/*If flush is enabled, empty the fetch1, fetch2 an decode stages*/
	if( flush ) 
	{ 
		IF1->st = EMPTY; 
		IF2->st = EMPTY; 
		ID->st = EMPTY; 
		/*Incase of predict taken and branch taken, don't flush execute stage, Bec, correct instruction is fetched after decode stage*/
		if(technique != 3) 
		{
			EX->st = EMPTY; 
		}
		flush = 0; 
		--inst_cycle; 
	}
     ++cycle;  
}


/*------------------------------------------------- printRegisters -----
         |  Function	printRegisters
         |
         |  Purpose:	All the register content is printed by this function
         |				Register[] is an integer array of 256 integers ti signify 256 registers
         |
		 |  Returns:	Void
         *-------------------------------------------------------------------*/

void printRegisters() 
{
	int j = 0;   /*local iteration variable*/
	fprintf(output_file, "\n");
	fprintf( output_file, "REGISTERS\n" );

for( j = 0; j < 256; ++j )
    {
      if( Register[j] != 0 )
		fprintf( output_file, "R%d %d\n", j, Register[j] );   /*print all registers content*/
    }
}


/*------------------------------------------------- printMemory -----
         |  Function	 printMemory
         |
         |  Purpose:	This function prints memory content of all registers
         |				memory[] is an array of 256 integers. all the memory content and address 
		 |				if not zero is printed to the output file. 
         |  Returns:	Void
         *-------------------------------------------------------------------*/

void printMemory() 
{
	int k = 0;			/*local iteration variable*/
	fprintf( output_file, "MEMORY\n" );
	for( k = 0; k < 256; ++k )
	{
		if( Memory[k] != 0 ) 
		{
			fprintf( output_file, "%d %d\n", (k * 8), Memory[k] ); /*print all memory content*/
		}
    }
}


/*------------------------------------------------- handleinput -----
         |  Function	handleinput
         |
         |  Purpose:	This function handles interactive input from user
         |				Prompts user to enter input file, output file and chose
		 |				Branch Prediction Technique
		 |
         |  Returns:	Void
         *-------------------------------------------------------------------*/

void handleinput() 
{
	char Inpname[20];
	char Otpname[20];
	printf("Enter Input file "); 
	//fflush(stdin);
	fgets(Inpname, MAX_STR, stdin);					/*get input file name by user interaction */
	sscanf(Inpname, "%s\n", Inpname);				/*read the file name entered*/

	printf("Enter Output file ");		
	fgets(Otpname, MAX_STR, stdin);					/*get output file name by user interaction */
	sscanf(Otpname, "%s\n", Otpname);				/*read output file name*/

	input_file = fopen(Inpname, "r+");				/*open input file in read mode*/
	output_file = fopen(Otpname, "w+");				/*open output file in write mode*/
	readInput(input_file, output_file);				/*read input data sections from user input file*/
	printf("Chose the technique: 1 - Flushing\n");	/*prompt user to chose the branch prediction technique*/
	printf("2 - Predict branch not taken,\n"); 
	printf("3 - predict branch taken \n");
	scanf("%d", &technique);						 /*read the chosen branch prediction technique*/
}

/*------------------------------------------------- handleinput -----
         |  Function handleinput
         |
         |  Purpose:  CLEARS BUFFER IF LEFT WITH ANY NEW LINE CHARACTERS BY SCANF
         |
         |  Returns:	Void
         *-------------------------------------------------------------------*/

void clean_stdin() {
int m = 0;
do {
	m = getchar();
} while(m!='\n' &&  m!=EOF);
}

/*------------------------------------------------- main -----
         |  Function main
         |
         |  Purpose:  THIS IS THE ENTRY POINT OF SIMULATOR. PIPELINE STAGES ARE CALLED IN REVERSE ORDER 
		 |  AND IN A LOOP FOR EVERY PROCESSOR CYCLE. THE LOOP IS EXECUTED UNTILL ALL THE INSTRUCTIONS ARE
		 |  PROCESSED AND WRITTEN BACK. TIMING STATISTICS ARE PRINTED TO OUTPUT FILE IN CORRESPONDING STAGES.
		 |  FINAL CONTENTS OF MEMORY AND REGISTERS ARE WRITTEN TO OUTPUT FILE OUTOF WHILE UNNECASSARY NEWLINE 
		 |  CHARACTER IN BUFFER IS FLUSHED, INPUT AND OUTPUT FILES ARE CLOSED AND USER IS PROMPTED AN OPTION 
		 |  WHETHER TO RUN ANOTHER SIMULATION OR EXIT
         |  
         |Parameters: argc, argv[] but command line arguments are not used
         |      
         |  Returns:	int - returns 0 to the process
         *-------------------------------------------------------------------*/

int main(int argc, char* argv[]) 
{
	char c;							/*variable to hold user decision on running another simulation*/
	int repeat;						/*Integer Variable declared to indicate whether user wants to RUN the simulation again*/
	do
	{
		init_pipeline();			/*Initialize the pipeline data, initially all stages empty and processor cycle 1*/
		handleinput();				/*Handle user input*/
	do 
	{
		writeOutput(output_file);	/*write simulation output statistics to output file*/
		writeBack(WB);				/*Write-back stage*/
		memoryStage3(MEM3);			/*memory access done at this stage*/
		memoryStage2(MEM2);			/*memory2 stage*/
		memoryStage1(MEM1);			/*memory1 stage*/
		execute(EX);				/*execute stage*/
		instructionDecode(ID);		/*instruction decode stage*/
		instructionFetch2(IF2);		/*instruction fetch 2 stage*/
		instructionFetch1(IF1);		/*instruction fetch 1 stage*/
		incrementCycle();			/*move the instruction down the pipeline*/
	} 
	/*repeat the process untill all the pipeline stages are empty*/
	while( ( (IF1->st != EMPTY) || (IF2->st != EMPTY) || (ID->st != EMPTY) || (EX->st != EMPTY) || 
		(MEM1->st != EMPTY) || (MEM2->st != EMPTY) || (MEM3->st != EMPTY) || (WB->st != EMPTY) ) );

	printRegisters();				/*print register numbers and register content*/
	printMemory();					/*print memory address and memory content*/
	fclose(input_file);				/*close the input file*/
	fclose(output_file);			/*close the output file*/
	printf("Would you like to continue? Y / N");
	/*flush stdin to clear any remaining newline characters in buffer*/
	clean_stdin();
	c = getchar();
	/*flush stdin to clear any remaining newline characters in buffer*/
	clean_stdin();
	/*read character from stdin: the decision whether to run another simulation*/
	repeat = ( c == 'y' || c == 'Y' ) ? 1 : 0;

	} while( repeat );				/*repeat simulation steps untill user wants to quit*/
	printf("Bye!");
	return 0;
}







