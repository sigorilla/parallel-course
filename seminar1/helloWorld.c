#include <mpi.h>
#include <stdio.h>

int main (int argc, char** argv) {

    MPI_Init(&argc, &argv);

    int size, my_rank;

    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    printf("Hello, World! I'm proc. %d of %d\n", my_rank, size);

    MPI_Finalize();
    return 0;
}
