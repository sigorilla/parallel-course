/**
 * qsort.c - Parallel sorting algorithm based on quicksort
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

#define N 10000

static inline
void swap(int *v, int i, int j) {
    int t = v[i];
    v[i] = v[j];
    v[j] = t;
}

void quicksort(int *v, int s, int n) {
    int x, p, i;
    // base case?
    if (n <= 1) {
        return;
    }
    // pick pivot and swap with first element
    x = v[s + n/2];
    swap(v, s, s + n/2);
    // partition slice starting at s+1
    p = s;
    for (i = s+1; i < s+n; i++) {
        if (v[i] < x) {
            p++;
            swap(v, i, p);
        }
    }
    // swap pivot into place
    swap(v, s, p);
    // recurse into partition
    quicksort(v, s, p-s);
    quicksort(v, p+1, s+n-p-1);
}

int *merge(int *v1, int n1, int *v2, int n2) {
    int *result = (int *) malloc((n1 + n2) * sizeof(int));
    int i = 0;
    int j = 0;
    int k;
    for (k = 0; k < n1 + n2; k++) {
        if (i >= n1) {
            result[k] = v2[j];
            j++;
        } else if (j >= n2) {
            result[k] = v1[i];
            i++;
        } else if (v1[i] < v2[j]) {
            result[k] = v1[i];
            i++;
        } else {
            result[k] = v2[j];
            j++;
        }
    }
    return result;
}

int main(int argc, char ** argv) {
    int n;
    int * data = NULL;
    int c, s;
    int * chunk;
    int o;
    int * other;
    int step;
    int p, id;
    MPI_Status status;
    double elapsed_time;
    int i;

    if (argc < 2) {
        fprintf(stderr, "Usage: mpirun -np <num_procs> %s <N>\n", argv[0]);
        exit(1);
    }

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &p);
    MPI_Comm_rank(MPI_COMM_WORLD, &id);

    if (id == 0) {
        n = (argc >= 2) ? atoi(argv[1]) : N;
        // compute chunk size
        c = (n % p != 0) ? n / p + 1 : n / p;
        data = (int *) malloc(p*c * sizeof(int));
        srand(time(NULL));
        for (i = 0; i < n; i++) {
            data[i] = rand();
        }
        for (i = n; i < p*c; i++) {
            data[i] = 0;
        }
    }

    MPI_Barrier(MPI_COMM_WORLD);
    elapsed_time = -MPI_Wtime();

    // broadcast size
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // compute chunk size
    c = (n % p != 0) ? n / p + 1 : n / p;

    // scatter data
    chunk = (int *) malloc(c * sizeof(int));
    MPI_Scatter(data, c, MPI_INT, chunk, c, MPI_INT, 0, MPI_COMM_WORLD);
    free(data);
    data = NULL;

    // compute size of own chunk and sort it
    s = (n >= c * (id+1)) ? c : n - c * id;
    quicksort(chunk, 0, s);

    // up to log_2 p merge steps
    for (step = 1; step < p; step = 2*step) {
        if (id % (2*step) != 0) {
            // id is no multiple of 2*step: send chunk to id-step and exit loop
            MPI_Send(chunk, s, MPI_INT, id-step, 0, MPI_COMM_WORLD);
            break;
        }
        // id is multiple of 2*step: merge in chunk from id+step (if it exists)
        if (id+step < p) {
            // compute size of chunk to be received
            o = (n >= c * (id+2*step)) ? c * step : n - c * (id+step);
            // receive other chunk
            other = (int *) malloc(o * sizeof(int));
            MPI_Recv(other, o, MPI_INT, id+step, 0, MPI_COMM_WORLD, &status);
            // merge and free memory
            data = merge(chunk, s, other, o);
            free(chunk);
            free(other);
            chunk = data;
            s = s + o;
        }
    }

    elapsed_time += MPI_Wtime();

    if (id == 0) {
        /* FILE * file = NULL;
        file = fopen("result.q.txt", "w");
        fprintf(file, "%d\n", s);
        for (i = 0; i < s; i++) {
            fprintf(file, "%d\n", chunk[i]);
        }
        fclose(file); */
        printf("\t%f", elapsed_time);
    }

    MPI_Finalize();
    return 0;
}
