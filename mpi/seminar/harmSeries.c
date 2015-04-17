#include <mpi.h>
#include <stdio.h>

int main (int argc, char** argv) {

    int world_size, world_rank;
    int N, N_master, N_slave;
    double startwtime = 0.0, endwtime;

    N = (argc >= 2) ? atoi(argv[1]) : 1000;

    double local_sum = 0.0, global_sum;

    MPI_Init(&argc, &argv);

    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    int count = 1, tag = 0;

    if (world_rank == 0) {
        startwtime = MPI_Wtime();

        N_slave = (int) (N / world_size);
        N_master = N_slave + N % world_size;

        int i = 1;
        while (i < world_size ) {
            int n_start = N_master + 1 + (i - 1) * N_slave;
            MPI_Send(&n_start, count, MPI_INT, i, tag, MPI_COMM_WORLD);
            i++;
        }

        int n = 1;
        while (n <= N_master) {
            local_sum += (1.0 / n);
            n++;
        }
    } else {
        int n = N_master + 1 + (world_rank - 1) * N_slave;
        int n_start = n;
        while (n < (n_start + N_slave)) {
            local_sum += (1.0 / n);
            n++;
        }
    }

    MPI_Reduce(&local_sum, &global_sum, count, MPI_DOUBLE,
            MPI_SUM, 0, MPI_COMM_WORLD);

    if (world_rank == 0) {
        endwtime = MPI_Wtime();

        printf("Sum of harmonical series 1/n = %.4lf for N = %d\n",
                global_sum, N);
        printf("Time: %lf s\n", (endwtime - startwtime) * 1000);
    }

    MPI_Finalize();
    return 0;
}

