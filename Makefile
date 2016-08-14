all: simulator

simulator:simulator.o simulator.h
	gcc -o simulator simulator.o

simulator.o:simulator.c
	gcc -c simulator.c
clean:
	rm simulator.o
