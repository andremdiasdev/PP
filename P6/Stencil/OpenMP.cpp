#include "hash.hpp"

double **__restrict x = malloc2D(jmax, imax);
double **__restrict xnew = malloc2D(jmax, imax);
double total_time = 0.0;
#pragma omp target enter data map(to : x[0 : jmax][0 : imax], xnew[0 : jmax][0 : imax])
#pragma omp target teams
{
#pragma omp distribute
    for (int j = 0; j < jmax; j++)
    {
#pragma omp parallel for simd
        for (int i = 0; i < imax; i++)
        {
            xnew[j][i] = 0.0;
            x[j][i] = 5.0;
        }
    }
#pragma omp distribute
    for (int j = jmax / 2 - 5; j < jmax / 2 + 5; j++)
    {
#pragma omp parallel for simd
        for (int i = imax / 2 - 5; i < imax / 2 - 1; i++)
        {
            x[j][i] = 400.0;
        }
    }
}
for (int iter = 0; iter < niter; iter += nburst)
{
    for (int ib = 0; ib < nburst; ib++)
    {
// 5-point stencil
#pragma omp target teams distribute
        for (int j = 1; j < jmax - 1; j++)
        {
#pragma omp parallel for simd
            for (int i = 1; i < imax - 1; i++)
            {
                xnew[j][i] = (x[j][i] + x[j][i - 1] + x[j][i + 1] +
                              x[j - 1][i] + x[j + 1][i]) /
                             5.0;
            }
        }
// Copy xnew → x
#pragma omp target teams distribute
        for (int j = 0; j < jmax; j++)
        {
#pragma omp parallel for simd
            for (int i = 0; i < imax; i++)
            {
                x[j][i] = xnew[j][i];
            }
        }
