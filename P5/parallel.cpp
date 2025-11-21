#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <chrono>
// TODO(OpenMP): include <omp.h> once you enable OpenMP and need timing/thread info.
// #include <omp.h>
      
int main() {
    const int mesh_size = 2;  // 2x2 coarse
    const int levmx     = 1;  // one refinement level
    const int ncells    = 7;  // 3 coarse + 4 fine

    const int ref_i0 = 1, ref_j0 = 0; // the refined coarse cell

    // Force OpenMP to use threads
    

    int *i_arr = (int *)malloc(ncells * sizeof(int));
    int *j_arr = (int *)malloc(ncells * sizeof(int));
    int *level = (int *)malloc(ncells * sizeof(int));

    // Build the 3 coarse level-0 cells
    int idx = 0;
    for (int j = 0; j < mesh_size; ++j)
        for (int i = 0; i < mesh_size; ++i)
            if (!(i == ref_i0 && j == ref_j0)) {
                i_arr[idx] = i;
                j_arr[idx] = j;
                level[idx] = 0;
                ++idx;
            }

    // Add 4 level-1 fine cells replacing the refined one
    for (int jj = 0; jj < 2; ++jj)
        for (int ii = 0; ii < 2; ++ii) {
            i_arr[idx] = ref_i0 * 2 + ii;
            j_arr[idx] = ref_j0 * 2 + jj;
            level[idx] = 1;
            ++idx;
        }

    int **hash, imax, jmax;
    Neigh *neigh;
    build_hash_and_neighbors(levmx, mesh_size, ncells, i_arr, j_arr, level,
                             &hash, &imax, &jmax, &neigh);

    std::cout << "imax=" << imax << " jmax=" << jmax << "\nHash table:\n";
    for (int j = jmax - 1; j >= 0; --j) {
        for (int i = 0; i < imax; ++i)
            std::cout << std::setw(3) << hash[j][i] << " ";
        std::cout << "\n";
    }

    // Keep original 2D arrays for final printing (not used in compute)
    double **x = malloc2D_double(jmax, imax);
    double **xnew = malloc2D_double(jmax, imax); // mirroring original
    double **xtmp;

    // Per-cell working arrays for hash-based stencil
    double *x_cell = (double *)malloc(ncells * sizeof(double));
    double *xnew_cell = (double *)malloc(ncells * sizeof(double));

    /************ Initialization timing ************/
    auto t_init_start = std::chrono::high_resolution_clock::now();

    // TODO(OpenMP): parallelize this initialization if desired.
    
    for (int ic = 0; ic < ncells; ++ic)
        x_cell[ic] = (level[ic] == 1) ? 400.0 : 0.0;

    auto t_init_end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> init_time = t_init_end - t_init_start;
    std::cout << "\nInitialization time: " << init_time.count()<< "  ";;

    /************ Stencil timing (hash-based neighbors on cells) ************/
    const int ITER = 100;
    auto t_stencil_start = std::chrono::high_resolution_clock::now();
    // TODO(OpenMP): define the parallel region
    for (int iter = 0; iter < ITER; ++iter) {
        
        // Typical pattern:
        // TODO(OpenMP): // parallelize the per-iteration update across cells.
        for (int ic = 0; ic < ncells; ++ic) {
            // TODO(Stencils): stencil update body (parallel-safe; only reads x_cell and writes xnew_cell[ic])
            double sum = x_cell[ic];
            int count = 1;
            if (neigh[ic].left  >= 0) { sum += x_cell[neigh[ic].left];  ++count; }
            if (neigh[ic].right >= 0) { sum += x_cell[neigh[ic].right]; ++count; }
            if (neigh[ic].bot   >= 0) { sum += x_cell[neigh[ic].bot];   ++count; }
            if (neigh[ic].top   >= 0) { sum += x_cell[neigh[ic].top];   ++count; }
            xnew_cell[ic] = sum / (double)count;
        }

        // TODO(OpenMP): a barrier is implicit at end of 'parallel for' unless 'nowait' used.
        // Swap pointers (serial op; keep outside parallel region).
        // TODO(OpenMP): if you keep the parallel region open, use single/master for swap.
        
    }

    auto t_stencil_end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> stencil_time = t_stencil_end - t_stencil_start;
    std::cout << "Stencil computation time (hash-based): " << stencil_time.count();

    /************ Scatter cell values back to fine grid for printing ************/
    // TODO(OpenMP): optional parallelization (independent writes).
    for (int j = 0; j < jmax; ++j)
        for (int i = 0; i < imax; ++i) {
            int ic = hash[j][i];
            x[j][i] = (ic >= 0) ? x_cell[ic] : 0.0;
        }


    free(i_arr); free(j_arr); free(level);
    free(hash); free(neigh);
    free(x); free(xnew);
    free(x_cell); free(xnew_cell);

    return 0;
}
