#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"
#include <cstdlib>
#include <iostream>
#include <string>

TEST_CASE("MPI output comparison between 1 and 2 processes", "[mpi]") {
    const std::string output1 = "run_mpi_1proc.out";
    const std::string output2 = "run_mpi_2proc.out";

    // Run with 1 process
    REQUIRE(std::system(("mpirun -n 1 ./MPITimeIt > " + output1).c_str()) == 0);

    // Run with 2 processes
    REQUIRE(std::system(("mpirun -n 2 ./MPITimeIt > " + output2).c_str()) == 0);

    // Compare outputs using numdiff
    int cmp_result = std::system(("numdiff -r1e-2 " + output1 + " " + output2).c_str());

    if (cmp_result != 0) {
        std::cerr << "\n[Error] Output mismatch between 1-proc and 2-proc runs.\n";
    }

    REQUIRE(cmp_result == 0);
}
