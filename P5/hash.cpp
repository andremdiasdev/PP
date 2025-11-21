#include "hash.hpp"

int **malloc2D(int jmax, int imax) {
    int **a = (int **)malloc(jmax * sizeof(int *) + (size_t)jmax * imax * sizeof(int));
    int *data = (int *)(a + jmax);
    for (int j = 0; j < jmax; ++j) a[j] = data + j * imax;
    return a;
}

double **malloc2D_double(int jmax, int imax) {
    double **a = (double **)malloc(jmax * sizeof(double *) + (size_t)jmax * imax * sizeof(double));
    double *data = (double *)(a + jmax);
    for (int j = 0; j < jmax; ++j) a[j] = data + j * imax;
    return a;
}

void build_hash_and_neighbors(
    int levmx, int mesh_size, int ncells,
    const int *i_arr, const int *j_arr, const int *level,
    int ***hash_out, int *imax_out, int *jmax_out, Neigh **neigh_out)
{
    int *levtable = (int *)malloc((levmx + 1) * sizeof(int));
    for (int lev = 0; lev <= levmx; ++lev) levtable[lev] = 1 << lev;

    const int scale_max = levtable[levmx];
    const int imax = mesh_size * scale_max;
    const int jmax = mesh_size * scale_max;

    int **hash = malloc2D(jmax, imax);
    for (int j = 0; j < jmax; ++j)
        for (int i = 0; i < imax; ++i)
            hash[j][i] = -1;

    // Fill hash table
    for (int ic = 0; ic < ncells; ++ic) {
        int lev = level[ic];
        int scale = levtable[levmx - lev];
        for (int jj = j_arr[ic] * scale; jj < (j_arr[ic] + 1) * scale; ++jj)
            for (int ii = i_arr[ic] * scale; ii < (i_arr[ic] + 1) * scale; ++ii)
                if (jj >= 0 && jj < jmax && ii >= 0 && ii < imax)
                    hash[jj][ii] = ic;
    }

    Neigh *neigh = (Neigh *)malloc(ncells * sizeof(Neigh));

    for (int ic = 0; ic < ncells; ++ic) {
        int lev = level[ic];
        int mult = levtable[levmx - lev];
        int i = i_arr[ic];
        int j = j_arr[ic];

        int x_mid = clamp(i * mult, 0, imax - 1);
        int y_mid = clamp(j * mult, 0, jmax - 1);

        int x_left  = clamp(i * mult - 1, 0, imax - 1);
        int x_right = clamp((i + 1) * mult, 0, imax - 1);
        int y_bot   = clamp(j * mult - 1, 0, jmax - 1);
        int y_top   = clamp((j + 1) * mult, 0, jmax - 1);

        neigh[ic].left  = hash[y_mid][x_left];
        neigh[ic].right = hash[y_mid][x_right];
        neigh[ic].bot   = hash[y_bot][x_mid];
        neigh[ic].top   = hash[y_top][x_mid];
    }

    *hash_out = hash;
    *imax_out = imax;
    *jmax_out = jmax;
    *neigh_out = neigh;
    free(levtable);
}
