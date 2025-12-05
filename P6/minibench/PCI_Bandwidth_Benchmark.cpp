#include <iostream>
#include <fstream>
#include <chrono>
#include <cuda.h>
#include <cuda_runtime.h>
#include <cassert>
#include <hash.hpp>

using clock_type = std::chrono::high_resolution_clock;
// -----------------------------------------------------------------------------
// Pageable Host → Device
// -----------------------------------------------------------------------------
void Host_to_Device_Pageable(int N, double *copy_time)
{
float *x_host, *x_device;
x_host = new float[N];
cudaMalloc((void**)&x_device, N * sizeof(float));
auto tstart = clock_type::now();
for (int i = 1; i <= 1000; i++) {
cudaMemcpy(x_device, x_host, N*sizeof(float), cudaMemcpyHostToDevice);
}
cudaDeviceSynchronize();
auto tend = clock_type::now();
auto dt = std::chrono::duration<double>(tend - tstart).count();
*copy_time = dt / 1000.0;
delete[] x_host;
cudaFree(x_device);
}
// -----------------------------------------------------------------------------
// Pinned Host → Device
// -----------------------------------------------------------------------------
void Host_to_Device_Pinned(int N, double *copy_time)
{
float *x_host, *x_device;
cudaError_t status = cudaMallocHost((void**)&x_host, N*sizeof(float));
if (status != cudaSuccess)
std::cerr << "Error allocating pinned host memory\n";
cudaMalloc((void**)&x_device, N*sizeof(float));
auto tstart = clock_type::now();
for (int i = 1; i <= 1000; i++) {
cudaMemcpy(x_device, x_host, N*sizeof(float), cudaMemcpyHostToDevice);
}
cudaDeviceSynchronize();
auto tend = clock_type::now();
auto dt = std::chrono::duration<double>(tend - tstart).count();
*copy_time = dt / 1000.0;
cudaFreeHost(x_host);
cudaFree(x_device);
}
// -----------------------------------------------------------------------------
// Pageable Experiment Loop
// -----------------------------------------------------------------------------
void H2D_Pageable_Experiments(double **bandwidth, int n_experiments, int max_array_size)
{
long long array_size;
double copy_time;
for (int j = 0; j < n_experiments; j++) {
array_size = 1;
for (int i = 0; i < max_array_size; i++) {
Host_to_Device_Pageable(array_size, &copy_time);
double byte_size = 4.0 * array_size;
bandwidth[j][i] = byte_size / (copy_time * 1024.0 * 1024.0 * 1024.0);
array_size *= 2;
}
}
}
// -----------------------------------------------------------------------------
// Pinned Experiment Loop
// -----------------------------------------------------------------------------
void H2D_Pinned_Experiments(double **bandwidth, int n_experiments, int max_array_size)
{
long long array_size;
double copy_time;
for (int j = 0; j < n_experiments; j++) {
array_size = 1;
for (int i = 0; i < max_array_size; i++) {
Host_to_Device_Pinned(array_size, &copy_time);
double byte_size = 4.0 * array_size;
bandwidth[j][i] = byte_size / (copy_time * 1024.0 * 1024.0 * 1024.0);
array_size *= 2;
}
}
}
// -----------------------------------------------------------------------------
// Mean + Variance
// -----------------------------------------------------------------------------
void Calculate_Mean_and_Variance(double **bandwidth,
double *mean_bandwidth,
double *variance_bandwidth,
int max_array_size,
int n_experiments)
{
for (int i = 0; i < max_array_size; i++) {
mean_bandwidth[i] = 0.0;
variance_bandwidth[i] = 0.0;
}
for (int j = 0; j < n_experiments; j++)
for (int i = 0; i < max_array_size; i++)
mean_bandwidth[i] += bandwidth[j][i] / n_experiments;
for (int j = 0; j < n_experiments; j++)
for (int i = 0; i < max_array_size; i++) {
double diff = bandwidth[j][i] - mean_bandwidth[i];
variance_bandwidth[i] += (diff * diff) / n_experiments;
}
}
// -----------------------------------------------------------------------------
// MAIN
// -----------------------------------------------------------------------------
int main()
{
    const int max_array_size = 28;
    const int n_experiments = 8;
    long long array_size;
    double **bandwidth =
    (double**)malloc2D(n_experiments, max_array_size);
    double *mean_bandwidth = new double[max_array_size];
    double *variance_bandwidth = new double[max_array_size];
    // -------------------------------------------------------------------------
    // Pageable Memory Tests
    // -------------------------------------------------------------------------
    H2D_Pageable_Experiments(bandwidth, n_experiments, max_array_size);
    Calculate_Mean_and_Variance(bandwidth, mean_bandwidth, variance_bandwidth,
    max_array_size, n_experiments);
    std::ofstream fp1("h2d_bandwidth_pageable.csv");
    std::cout << "h2d_bandwidth_pageable\n";
    fp1 << "Array Size (B), Bandwidth Mean (GB/s), Bandwidth Variance (GB/s)\n";
    std::cout << "Array Size (B), Bandwidth Mean (GB/s), Bandwidth Variance (GB/s)\n";
    array_size = 1;
    for (int i = 0; i < max_array_size; i++) {
    double byte_size = 4.0 * array_size;
    fp1 << byte_size << ", " << mean_bandwidth[i] << ", " << variance_bandwidth[i] << "\n";
    std::cout << byte_size << ", " << mean_bandwidth[i] << ", " << variance_bandwidth[i] << "\n";
    array_size *= 2;
    }
    fp1.close();
    // -------------------------------------------------------------------------
    // Pinned Memory Tests
    // -------------------------------------------------------------------------
    H2D_Pinned_Experiments(bandwidth, n_experiments, max_array_size);
    Calculate_Mean_and_Variance(bandwidth, mean_bandwidth, variance_bandwidth,
    max_array_size, n_experiments);
    std::ofstream fp2("h2d_bandwidth_pinned.csv");
    std::cout << "\nh2d_bandwidth_pinned\n";
    fp2 << "Array Size (B), Bandwidth Mean (GB/s), Bandwidth Variance (GB/s)\n";
    std::cout << "Array Size (B), Bandwidth Mean (GB/s), Bandwidth Variance (GB/s)\n";
    array_size = 1;
    for (int i = 0; i < max_array_size; i++) {
    double byte_size = 4.0 * array_size;
    fp2 << byte_size << ", " << mean_bandwidth[i] << ", " << variance_bandwidth[i] << "\n";
    std::cout << byte_size << ", " << mean_bandwidth[i] << ", " << variance_bandwidth[i] << "\n";
    array_size *= 2;
}
fp2.close();
// -------------------------------------------------------------------------
// Cleanup
// -------------------------------------------------------------------------
free(bandwidth); // If your malloc2D requires zero offset
delete[] mean_bandwidth;
delete[] variance_bandwidth;

return 0;
}