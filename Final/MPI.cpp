#include <mpi.h>
#include <iostream>

int main(int argc, char** argv) {
    //TODO 1:
    //Initialize MPI and get rank and nprocs.
    MPI_Init(&argc, &argv);

    int rank = 0, nprocs = 0;

    //TODO 2:
    //Use MPI_Comm_rank and MPI_Comm_size to set rank and nprocs.
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

    int local_val = rank * rank;

    int global_sum = 0;
    int global_max = 0;

    //TODO 3:
    //Use MPI_Allreduce to compute:
    // - global_sum: sum of local_val over all ranks
    // - global_max: maximum of local_val over all ranks
    MPI_Allreduce(&local_val, &global_sum, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
    MPI_Allreduce(&local_val, &global_max, 1, MPI_INT, MPI_MAX, MPI_COMM_WORLD);

    //TODO 4:
    //Print rank, local_val, global_sum, and global_max, e.g.:
    //Rank X: local = ..., sum = ..., max = ...
    std::cout << "Rank " << rank
    << ": local_val = " << local_val
    << ", global_sum = " << global_sum
    << ", global_max = " << global_max
    << std::endl;

    //TODO 5:
    //Finalize MPI.
    MPI_Finalize();

    return 0;
}