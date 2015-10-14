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
    MPI_Status status;

    double t_start = MPI_Wtime();

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
        if (rank != 0) {
            MPI_Send(a[i], JSIZE, MPI_DOUBLE, 0, i, MPI_COMM_WORLD);
        }
    }

    if (rank == 0) {

        double t_end = MPI_Wtime();
        printf("time: %lf\n", t_end - t_start);
        //FILE *ff;

        //ff = fopen("result.parallel.txt", "w");
        for (i = start; i < end; i++) {
            for (j = 0; j < JSIZE; j++) {
                //fprintf(ff, "%f ", a[i][j]);
            }
            //fprintf(ff, "\n");
        }

        int k;
        for (k = end; k < ISIZE; k++) {
            double tmp[JSIZE];
            int tmp_rank = (int) (k / divN);
            tmp_rank = (tmp_rank == size) ? tmp_rank - 1 : tmp_rank;
            MPI_Recv(tmp, JSIZE, MPI_DOUBLE, tmp_rank, k, MPI_COMM_WORLD, &status);
            for (j = 0; j < JSIZE; j++) {
                //fprintf(ff, "%f ", tmp[j]);
            }
            //fprintf(ff, "\n");
        }

        //fclose(ff);
    }

    MPI_Finalize();

    return 0;
}
