#include <mpi.h>
#include <stdio.h>

int main (int argc, char** argv) {

    MPI_Init(&argc, &argv);

    int world_size, world_rank;

    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    int token;
    int count = 1, tag = 0;

    if (world_rank != 0) {
        MPI_Recv(&token, count, MPI_INT, world_rank - 1, tag,
                MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("Proc. %d received token %d from proc. %d\n",
                world_rank, token, world_rank - 1);
    } else {
        token = 213;
    }
    MPI_Send(&token, count, MPI_INT, (world_rank + 1) % world_size,
            tag, MPI_COMM_WORLD);

    if (world_rank == 0) {
        MPI_Recv(&token, count, MPI_INT, world_size - 1, tag,
                MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("Proc. %d received token %d from proc. %d\n",
                world_rank, token, world_size - 1);
    }

    MPI_Finalize();
    return 0;
}
