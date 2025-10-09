#include <iostream>
#include <thread>
#include <chrono>
#include <mpi.h>

int main(int argc, char* argv[]) {
    int rank;

    MPI_Init(&argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    double start_time = MPI_Wtime();

    std::this_thread::sleep_for(std::chrono::seconds(1));

    double end_time = MPI_Wtime();
    
    if (rank == 0) {
        std::cout << "Elapsed time is " << (end_time-start_time) << " seconds.\n";
    }

    MPI_Finalize();

    return 0;
}