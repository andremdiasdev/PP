#include <iostream>
#include <chrono>
#include <cstdlib>
//#include "malloc2D.h"
#ifdef LIKWID_PERFMON
    #include "likwid.h"
#endif
#define REGION_NAME "STENCIL"
double **malloc2D(int jmax, int imax)
{
   // first allocate a block of memory for the row pointers and the 2D array
   double **x = (double **)malloc(jmax*sizeof(double *) + jmax*imax*sizeof(double));

   // Now assign the start of the block of memory for the 2D array after the row pointers
   x[0] = (double *)(x + jmax);

   // Last, assign the memory location to point to for each row pointer
   for (int j = 1; j < jmax; j++) {
      x[j] = x[j-1] + imax;
   }

   return(x);
}

#define SWAP_PTR(xnew,xold,xtmp) (xtmp=xnew, xnew=xold, xold=xtmp)

int main(int argc, char *argv[])
{
   
   #ifdef LIKWID_PERFMON
    LIKWID_MARKER_INIT;
    LIKWID_MARKER_REGISTER(REGION_NAME);
   #endif
    
    using clock_type = std::chrono::high_resolution_clock;
    using duration_type = std::chrono::duration<double>;

    double cpu_time = 0.0;
    int imax = 2002, jmax = 2002;

    double **xtmp, *xnew1d, *x1d;
    double **x = malloc2D(jmax, imax);
    double **xnew = malloc2D(jmax, imax);

    // keep malloc for flush
    int *flush = (int *)malloc(jmax * imax * sizeof(int) * 10);

    xnew1d = xnew[0]; 
    x1d = x[0];

    for (int i = 0; i < imax * jmax; i++) {
        xnew1d[i] = 0.0; 
        x1d[i] = 5.0;
    }

    for (int j = jmax/2 - 5; j < jmax/2 + 5; j++) {
        for (int i = imax/2 - 5; i < imax/2 - 1; i++) {
            x[j][i] = 400.0;
        }
    }

    for (int iter = 0; iter < 10000; iter++) {
        for (int l = 1; l < jmax * imax * 10; l++) {
            flush[l] = 1.0;
        }

        auto tstart_cpu = clock_type::now();
        #ifdef LIKWID_PERFMON
          LIKWID_MARKER_START(REGION_NAME);
        #endif

        // TODO: Implement the stencil operation

        #ifdef LIKWID_PERFMON
           LIKWID_MARKER_STOP(REGION_NAME);
        #endif 
        auto tstop_cpu = clock_type::now();

        cpu_time += std::chrono::duration_cast<duration_type>(tstop_cpu - tstart_cpu).count();

        SWAP_PTR(xnew, x, xtmp);

        if (iter % 100 == 0) {
            std::cout << "Iter " << iter << std::endl;
        }
    }

    std::cout << "Timing is " << cpu_time << " seconds" << std::endl;

    free(x);
    free(xnew);
    free(flush);
    #ifdef LIKWID_PERFMON
        LIKWID_MARKER_CLOSE;
    #endif
    return 0;
}


