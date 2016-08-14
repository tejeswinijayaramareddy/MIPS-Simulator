8 sstage MIPS Integer pipeline Simulator
========================================

1. This simulator is implemented to read a specific sequence of MIPS instructions from text file given by the user.
2. It calculates timing information for given instrucctions and outputs the formatted timing into a text file.

3. Instructions supported are:
	load double word (e.g., LD R2, 0(R1)),
	store double word (e.g., SD R4, 8(R3)),
	arithmetic operation add (e.g., DADD R1, R2, R3),
	arithmetic operations: sub (e.g., DSUB R1, R2, R3),
	control-flow instruction: BNEZ R4, Loop
	
	
4. Supported addressing modes are:
	register, register indirect, displacement, and immediate.
	
4. Eight stages of pipeline are taken to be :
	eight-stage pipeline: IF1, IF2, ID, EX, MEM1, MEM2, MEM3, and WB.
	
5. Branch instruction instruction computes the address of target instruction in ID and checks the condition in EX.

6. The load/store instructions complete the data cache access in MEM2.

7. Sample input has to as below:

	REGISTERS
	R1 16
	R3 42
	R5 8
	MEMORY 
	8 40
	16 60
	CODE
		LD R2, 0(R1)
		DADD R4, R2, R3
		SD R4, 0(R1)
		BNEZ R4, NEXT
		DADD R2, R1, #8
	NEXT: DADD R1, R1, R3
	
8. Sample interactive behavior of program:

	Enter Input file input.txt
	Enter Output file output.txt
	Chose the technique: 1 - Flushing
	2 - Predict branch not taken,
	3 - predict branch taken
	
9. Sample output:

	
	c#1 I1-IF1 
	c#2 I1-IF2 I2-IF1 
	c#3 I1-ID I2-IF2 I3-IF1 
	c#4 I1-EX I2-ID I3-IF2 I4-IF1 
	c#5 I1-MEM1 I2-stall I3-stall I4-stall 
	c#6 I1-MEM2 I2-stall I3-stall I4-stall 
	c#7 I1-MEM3 I2-EX I3-ID I4-IF2 I5-IF1 
	c#8 I1-WB I2-MEM1 I3-EX I4-ID I5-IF2 I6-IF1 
	c#9 I2-MEM2 I3-MEM1 I4-EX I5-ID I6-IF2 
	c#10 I2-MEM3 I3-MEM2 I4-MEM1 I7-IF1 
	c#11 I2-WB I3-MEM3 I4-MEM2 I7-IF2 
	c#12 I3-WB I4-MEM3 I7-ID 
	c#13 I4-WB I7-EX 
	c#14 I7-MEM1 
	c#15 I7-MEM2 
	c#16 I7-MEM3 
	c#17 I7-WB 
	REGISTERS
	R1 58
	R2 60
	R3 42
	R4 102
	R5 8
	MEMORY
	8 40
	16 102



	