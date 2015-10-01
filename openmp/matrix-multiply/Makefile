CC=gcc
CFLAGS=-fopenmp

all: MatrixC MatrixT

MatrixC: matrix-multiply-common.c
        $(CC) $(CFLAGS) -o exec-common.out matrix-multiply-common.c

MatrixT: matrix-multiply-transpose.c
        $(CC) $(CFLAGS) -o exec-transpose.out matrix-multiply-transpose.c
