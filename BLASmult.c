#include "globals.h"
#include "mkl.h"

#define BILLION 1E9

/* This routine performs a sgemm operation
 *  C := C + A * B
 * On exit, A and B maintain their input values.    
 * This function wraps a call to the BLAS-3 routine SGEMM, via the standard FORTRAN interface - hence the reference semantics. */
void square_sgemm(float A[], float B[], float C[])
{
    float alpha = 1.0;
    float beta = 1.0;
    struct timespec start, end;
    float seconds;


    int max_threads = 49;
    mkl_set_num_threads(max_threads);

    printf(" Running Intel(R) MKL from 1 to %i threads \n\n", max_threads);



    printf (" Computing matrix product using Intel(R) MKL dgemm function via CBLAS interface \n\n");
    clock_gettime(CLOCK_MONOTONIC, &start);

    cblas_sgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, 
                ndim, ndim, ndim, alpha, A, ndim, B, ndim, beta, C, ndim);


    clock_gettime(CLOCK_MONOTONIC, &end);
    seconds = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / BILLION;
    printf ("\n Computations completed in %f.\n\n", seconds);
}   

