#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>

#define ISIZE 1000
#define JSIZE 1000

double a[ISIZE][JSIZE];

int main(int argc, char **argv) {
    int rank;
    int size;

    MPI_Init( &argc, &argv );
    MPI_Comm_rank( MPI_COMM_WORLD, &rank );
    MPI_Comm_size( MPI_COMM_WORLD, &size );

    int divN = (int) (ISIZE / size);
    int modN = ISIZE % size;

    int start = rank * divN;
    int end = (rank + 1) * divN;
    if (rank == size - 1) {
        end += modN;
    }

    int i, j;

    for (i = start; i < end; i++) {
        for (j = 0; j < JSIZE; j++) {
            a[i][j] = 10 * i + j;
        }
    }
    for (i = start; i < end; i++) {
        for (j = 0; j < JSIZE; j++) {
            a[i][j] = sin(0.00001 * a[i][j]);
        }
    }

    MPI_Barrier(MPI_COMM_WORLD);

    if (rank == 0) {

        FILE *ff;

        ff = fopen("result.parallel.txt", "w");
        for (i = 0; i < ISIZE; i++) {
            for (j = 0; j < JSIZE; j++) {
                fprintf(ff, "%f ", a[i][j]);
            }
            fprintf(ff, "\n");
        }

        fclose(ff);
    }

    MPI_Finalize();

    return 0;
}
