#include <iostream>
#include <vector>
#include <omp.h>

int main(){
    constexpr int N = 1000;

    std::vector<std::vector<double>> A(N, std::vector<double>(N));
    std::vector<double> x(N), y(N,0.0);

    //TODO 1:
    //Use one or more OpenMP parallel for loops to:
    // -Initialize A[i][j] = i+j
    // -Initialize x[i] = 1.0
    // You may use nested loops with 'collapse' if you wish.

    #pragma omp parallel for collapse(2)
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            A[i][j] = static_cast<double>(i + j);
        }
    }

    #pragma omp parallel for
    for (int i = 0; i < N; i++) {
        x[i] = 1.0;
    }

    //TODO 2:
    // Compute y=A*x in parallel
    // Each i in [0,N] should compute:
    // y[i] = sum_{j=0}^{N-1} A[i][j] * x[j];
    // Use an OpenMP parallel for over i. You may use a private 
    //temporary variable for the inner sum

    #pragma omp parallel for
    for (int i = 0; i < N; i++) {
        double sum = 0.0;
        for (int j = 0; j < N; j++) {
            sum += A[i][j] * x[j];
        }
        y[i] = sum;
    }

    //TODO 3:
    // Print y[0] and y[N-1] in the format:
    // y[0] = <value>, y[N-1] = <value>

    std::cout << "y[0] = " << y[0]
    << ", y[" << N-1 << "] = " << y[N-1] << "\n";

    return 0;
}