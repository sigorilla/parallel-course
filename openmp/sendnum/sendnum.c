#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

#define DEF_N 10
#define N_THREADS 3

int main(int argc, char **argv) {
    int N, nthreads, curr_thread = -1;
    int i, tid;
    double sum = 0;

    if (argc > 1) {
        N = atoi(argv[1]);
        nthreads = atoi(argv[2]);
    } else {
        N = DEF_N;
        nthreads = N_THREADS;
    }

    omp_lock_t writelock;
    omp_set_num_threads(nthreads);
    omp_init_lock(&writelock);

    #pragma omp parallel shared(N, curr_thread) private(tid)
    {
        tid = omp_get_thread_num();

        if (tid == 0) {
            omp_set_lock(&writelock);
            N += 1;
            curr_thread = tid;
            omp_unset_lock(&writelock);
        } else {
            while (curr_thread < tid - 1) {
                continue;
            }
            omp_set_lock(&writelock);
            printf("Thread %d received token %d from token %d\n",
                    tid, N, tid - 1);
            N += 1;
            curr_thread = tid;
            omp_unset_lock(&writelock);
        }

        if (tid == 0) {
            while (curr_thread < nthreads - 1) {
                continue;
            }
            printf("Thread %d received token %d from token %d\n",
                    tid, N, nthreads - 1);
        }
    }

    omp_destroy_lock(&writelock);

    return 0;
}
