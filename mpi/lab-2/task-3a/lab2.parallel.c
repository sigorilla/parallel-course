#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>

#define ISIZE 1000
#define JSIZE 1000

double a[ISIZE][JSIZE], b[ISIZE][JSIZE];

int main(int argc, char **argv) {
    int rank;
    int size;

    MPI_Init( &argc, &argv );
    MPI_Comm_rank( MPI_COMM_WORLD, &rank );
    MPI_Comm_size( MPI_COMM_WORLD, &size );
    MPI_Status status;

    int divN = (int) (JSIZE / size);
    int modN = JSIZE % size;

    int start = rank * divN;
    int end = (rank + 1) * divN;
    if (rank == size - 1) {
        end += modN;
    }

    int i, j;

    for (i = 0; i < ISIZE; i++) {
        for (j = 0; j < JSIZE; j++) {
            a[i][j] = 10 * i + j;
            b[i][j] = 0.0;
        }
    }

    for (i = 0; i < ISIZE; i++) {
        for (j = start; j < end; j++) {
            a[i][j] = sin(0.00001 * a[i][j]);
        }
    }

    double t_start = MPI_Wtime();
    for (i = 0; i < ISIZE - 1; i++) {
        for (j = start; j < end; j++) {
            b[i][j] = a[i + 1][j] * 1.5;
        }
        if (rank != 0) {
            MPI_Send(&b[i][start], end - start, MPI_DOUBLE, 0, i, MPI_COMM_WORLD);
        }
    }

    if (rank == 0) {
        double t_end = MPI_Wtime();
        printf("%d\t%lf\n", size, t_end - t_start);

        if (size > 1) {
            for (i = 0; i < ISIZE - 1; i++) {
                for (j = end; j < JSIZE - modN; j += divN) {
                    int tmp_rank = (int) (j / divN);
                    tmp_rank = (tmp_rank == size) ? tmp_rank - 1 : tmp_rank;
                    if (tmp_rank == size - 1) {
                        MPI_Recv(&b[i][j], divN + modN, MPI_DOUBLE, tmp_rank, i, MPI_COMM_WORLD, &status);
                    } else {
                        MPI_Recv(&b[i][j], divN, MPI_DOUBLE, tmp_rank, i, MPI_COMM_WORLD, &status);
                    }
                }
            }
        }

        // FILE *ff;
        // ff = fopen("result.parallel.txt", "w");
        // for (i = 0; i < ISIZE; i++) {
        //     for (j = 0; j < JSIZE; j++) {
        //         fprintf(ff, "%f ", b[i][j]);
        //     }
        //     fprintf(ff, "\n");
        // }
        // fclose(ff);
    }

    MPI_Finalize();

    return 0;
}
