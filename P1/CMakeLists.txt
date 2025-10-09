cmake_minimum_required(VERSION 3.10)

#Set the project name and language
project(MPIApp)

# Use modern C++ (slides mention C++20)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find MPI
find_package(MPI REQUIRED)

#Include the directory for headers (if any)
include_directories(${CMAKE_SOURCE_DIR}/include)

# App
add_executable(MPITimeIt src/MPITimeIt.cpp)

# Link MPI libraries
target_link_libraries(MPITimeIt PRIVATE MPI::MPI_CXX)

#Include MPI headers
target_include_directories(MPITimeIt PRIVATE ${MPI_CXX_INCLUDE_PATH})

# Enable testing
enable_testing()

# Add the test executable
add_executable(my_test tests/test.cpp)

# Include directories for the test target
target_link_libraries(my_test PRIVATE MPI::MPI_CXX)

# Register the test executable with CTest (optional)
add_test(NAME my_test COMMAND my_test)