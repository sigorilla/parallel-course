CC=gcc
MPICC=mpicc
CFLAGS=-lm

all: Simple Parallel

Simple: lab1.simple.c 
	$(CC) $(CFLAGS) -o lab1.simple.out lab1.simple.c

Parallel: lab1.parallel.c 
	$(MPICC) $(CFLAGS) -o lab1.parallel.out lab1.parallel.c

