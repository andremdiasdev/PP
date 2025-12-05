#include "hash.hpp"

double **__restrict xtmp;
double **__restrict x = malloc2D(jmax, imax);
double **__restrict xnew = malloc2D(jmax, imax);
// Allocate on GPU
#pragma acc enter data create(x[0 : jmax][0 : imax], xnew[0 : jmax][0 : imax])
// Initialize arrays
#pragma acc parallel loop collapse(2) present(x[0 : jmax][0 : imax], xnew[0 : jmax][0 : imax])
for (int j = 0; j < jmax; j++)
{
    for (int i = 0; i < imax; i++)
    {
        xnew[j][i] = 0.0;
        x[j][i] = 5.0;
    }
}
// Central heat source
#pragma acc parallel loop collapse(2) present(x[0 : jmax][0 : imax])
for (int j = jmax / 2 - 5; j < jmax / 2 + 5; j++)
{
    for (int i = imax / 2 - 5; i < imax / 2 - 1; i++)
    {
        x[j][i] = 400.0;
    }
}
// Time-stepping loop
for (int iter = 0; iter < niter; iter += nburst)
{
    for (int ib = 0; ib < nburst; ib++)
    {
// Stencil computation
#pragma acc parallel loop tile(*, *) present(x[0 : jmax][0 : imax], xnew[0 : jmax][0 : imax])
        for (int j = 1; j < jmax - 1; j++)
        {
            for (int i = 1; i < imax - 1; i++)
            {
                xnew[j][i] = (x[j][i] + x[j][i - 1] + x[j][i + 1] + x[j - 1][i] + x[j + 1][i]) / 5.0;
            }
        }
        // Swap host pointers
        SWAP_PTR(xnew, x, xtmp);
// Swap device pointers
#pragma acc host_data use_device(x, xnew)
        {
            double **d_tmp = x;
            x = xnew;
            xnew = d_tmp;
        }
    }
// Free device data
#pragma acc exit data delete (x[0 : jmax][0 : imax], xnew[0 : jmax][0 : imax])