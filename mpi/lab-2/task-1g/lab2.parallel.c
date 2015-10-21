#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>

#define ISIZE 1000
#define JSIZE 1000
#define IN 1
#define JN 3

int main(int argc, char **argv) {
    double a[ISIZE][JSIZE];
    int rank;
    int size;

    MPI_Init( &argc, &argv );
    MPI_Comm_rank( MPI_COMM_WORLD, &rank );
    MPI_Comm_size( MPI_COMM_WORLD, &size );
    MPI_Status status;

    int divN = (int) ( (JSIZE - JN) / size);
    int modN = (JSIZE - JN) % size;

    int start = rank * divN;
    int end = (rank + 1) * divN;
    if (rank == size - 1) {
        end += modN;
    }

    int i, j, k;

    for (i = 0; i < ISIZE; i++) {
        for (j = 0; j < JSIZE; j++) {
            a[i][j] = 10 * i + j;
            if (size == 1 && i > IN && j > JN) {
                a[i][j] = sin(0.00001 * a[i - IN][j - JN]);
            }
        }
    }

    double t_start = MPI_Wtime();
    for (i = IN; i < ISIZE; i++) {
        for (j = end + JN - 1; j >= start + JN; j--) {
            if (size > 1) {
                if (rank == 0) {
                    a[i][j] = sin(0.00001 * a[i - IN][j - JN]);
                    if (j >= end + JN - 3) {
                        MPI_Send(&a[i][j], 1, MPI_DOUBLE, rank + 1, j, MPI_COMM_WORLD);
                    }
                }
                if (rank > 0 && rank < size - 1) {
                    if (j <= start + JN + 2 && i != IN) {
                        MPI_Recv(&a[i - IN][j - JN], 1, MPI_DOUBLE, rank - 1, j - JN, MPI_COMM_WORLD, &status);
                    }
                    a[i][j] = sin(0.00001 * a[i - IN][j - JN]);
                    if (j >= end + JN - 3) {
                        MPI_Send(&a[i][j], 1, MPI_DOUBLE, rank + 1, j, MPI_COMM_WORLD);
                    }
                }
                if (rank == size - 1) {
                    if (j <= start + JN + 2 && i != IN) {
                        MPI_Recv(&a[i - IN][j - JN], 1, MPI_DOUBLE, rank - 1, j - JN, MPI_COMM_WORLD, &status);
                    }
                    a[i][j] = sin(0.00001 * a[i - IN][j - JN]);
                }
            }
        }
        if (rank != 0) {
            MPI_Send(&a[i][start + JN], end - start, MPI_DOUBLE, 0, i, MPI_COMM_WORLD);
        }
    }

    if (rank == 0) {
        double t_end = MPI_Wtime();
        printf("%d\t%lf\n", size, t_end - t_start);

        if (size > 1) {
            for (i = IN; i < ISIZE; i++) {
                for (j = end + JN; j < JSIZE - modN; j += divN) {
                    int tmp_rank = (int) (j / divN);
                    tmp_rank = (tmp_rank == size) ? tmp_rank - 1 : tmp_rank;
                    if (tmp_rank == size - 1) {
                        MPI_Recv(&a[i][j], divN + modN, MPI_DOUBLE, tmp_rank, i, MPI_COMM_WORLD, &status);
                    } else {
                        MPI_Recv(&a[i][j], divN, MPI_DOUBLE, tmp_rank, i, MPI_COMM_WORLD, &status);
                    }
                }
            }
        }

        // FILE *ff;
        // ff = fopen("result.parallel.txt", "w");
        // for (i = 0; i < ISIZE; i++) {
        //     for (j = 0; j < JSIZE; j++) {
        //         fprintf(ff, "%f ", a[i][j]);
        //     }
        //     fprintf(ff, "\n");
        // }
        // fclose(ff);
    }

    MPI_Finalize();

    return 0;
}
