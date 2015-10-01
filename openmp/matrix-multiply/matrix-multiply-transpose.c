#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

int main(int argc, char **argv) {

    int N, M, P;
    // Size of arrays
    N = atoi(argv[1]);
    M = atoi(argv[2]);
    P = atoi(argv[3]);

    // Count of threads
    int L = atoi(argv[4]);

    omp_set_num_threads(L);

    int i, j, k;
    double A[N][M], B[M][P], C[N][P], D[P][M];

    #pragma omp parallel shared(A, B, C) private(i, j, k)
    {
        // Initializing arrays
        #pragma omp for schedule(static)
        for (i = 0; i < N; i++) {
            for (j = 0; j < M; j++) {
                A[i][j] = i+j;
            }
        }

        #pragma omp for schedule(static)
        for (i = 0; i < M; i++) {
            for (j = 0; j < P; j++) {
                B[i][j] = i*j;
            }
        }

        #pragma omp for schedule(static)
        for (i = 0; i < N; i++) {
            for (j = 0; j < P; j++) {
                C[i][j] = 0;
            }
        }

        // Use it for transpose
        #pragma omp for schedule(static)
        for (i = 0; i < M; i++) {
            for (j = 0; j < P; j++) {
                D[i][j] = B[j][i];
            }
        }

        // Solve result
        #pragma omp for schedule(static)
        for (i = 0; i < N; i++) {
            for (j = 0; j < P; j++) {
                for (k = 0; k < M; k++) {
                    C[i][j] += A[i][k] * D[j][k];
                }
            }
        }
    }

    // Print matrix
    /*
    for (i = 0; i < N; i++) {
        for (j = 0; j < P; j++) {
           printf("%.3lf ", C[i][j]);
        }
        printf("\n");
    }
    */

    return 0;
}
