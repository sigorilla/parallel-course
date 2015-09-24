#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

#define DEF_N 10
#define N_THREADS 10

int main(int argc, char **argv) {
    int N, nthreads;
    int i;
    double sum = 0;

    if (argc > 1) {
        N = atoi(argv[1]);
        nthreads = atoi(argv[2]);
    } else {
        N = DEF_N;
        nthreads = N_THREADS;
    }

    omp_set_num_threads(nthreads);

    #pragma omp parallel for reduction(+:sum)
        for (i = 1; i < N; i++) {
            sum += 1.0/i;
        }

    printf("Sum of harmonical series 1/n = %.4lf for N = %d\n", sum, N);

    return 0;
}
