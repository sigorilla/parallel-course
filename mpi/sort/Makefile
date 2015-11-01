CC=gcc
MPICC=mpicc
CFLAGS=-Wall -O -lm

all: bsort qsort

bsort: bsort.c
	$(MPICC) $(CFLAGS) -o bsort.out bsort.c

qsort: qsort.c
	$(MPICC) $(CFLAGS) -o qsort.out qsort.c

clean:
	rm -f *.out
