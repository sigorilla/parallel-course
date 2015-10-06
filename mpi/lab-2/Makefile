CC=gcc
MPICC=mpicc
CFLAGS=-lm

all: Simple Parallel

Simple: lab2.simple.c 
	$(CC) $(CFLAGS) -o lab2.simple.out lab2.simple.c

Parallel: lab2.parallel.c 
	$(MPICC) $(CFLAGS) -o lab2.parallel.out lab2.parallel.c

