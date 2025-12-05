#ifndef HASH_HPP
#define HASH_HPP

#include <cstdlib>

struct Neigh { int left, right, bot, top; };

inline int clamp(int v, int lo, int hi) { return v < lo ? lo : (v > hi ? hi : v); }

/************ single-malloc 2D array for int ************/
int **malloc2D(int jmax, int imax);

/************ allocate contiguous double arrays ************/
double **malloc2D_double(int jmax, int imax);

/************ build perfect hash and neighbors ************/
void build_hash_and_neighbors(
    int levmx, int mesh_size, int ncells,
    const int *i_arr, const int *j_arr, const int *level,
    int ***hash_out, int *imax_out, int *jmax_out, Neigh **neigh_out);

#endif // HASH_HPP
