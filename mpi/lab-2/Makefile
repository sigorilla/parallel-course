CC=gcc
MPICC=mpicc
CFLAGS=-lm

all: preSimple preParallel

preSimple: prelab2.simple.c 
	$(CC) $(CFLAGS) -o prelab2.simple.out prelab2.simple.c

preParallel: prelab2.parallel.c 
	$(MPICC) $(CFLAGS) -o prelab2.parallel.out prelab2.parallel.c

