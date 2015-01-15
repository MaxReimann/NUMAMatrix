#include <xmmintrin.h>
#include <stdio.h>
#include <time.h>
#include <time.h>

#include <stdlib.h>
#define BILLION 1E9
#define IDX(Y, X) (ndim * Y + X) //rows first

const int ndim = 2000;

void blockedMultiply(double A[], double B[], double C[]);

int main(int argc, char **argv)
{


    srand(1);
    double *first = malloc(ndim * ndim * sizeof(double));
    double *second = malloc(ndim * ndim * sizeof(double));
    double *multiply = malloc(ndim * ndim * sizeof(double));
    int i, c, d;
    printf("ndim: %d\n", ndim);
    setbuf(stdout, NULL);


    for (c = 0 ; c < ndim; c++)
    {
        for (d = 0 ; d < ndim; d++)
        {
            first[IDX(c, d)] = rand() % 10; //int between 0 - 1000
            second[IDX(c, d)] = rand() % 10;
            multiply[IDX(c, d)] = 0;

        }
    }

    blockedMultiply(first,second,multiply);
}


void inline M4x4_SSE(float *A, float *B, float *C) {
    __m128 row1 = _mm_load_ps(&B[0]);
    __m128 row2 = _mm_load_ps(&B[4]);
    __m128 row3 = _mm_load_ps(&B[8]);
    __m128 row4 = _mm_load_ps(&B[12]);
    for(int i=0; i<4; i++) {
        __m128 brod1 = _mm_set1_ps(A[4*i + 0]);
        __m128 brod2 = _mm_set1_ps(A[4*i + 1]);
        __m128 brod3 = _mm_set1_ps(A[4*i + 2]);
        __m128 brod4 = _mm_set1_ps(A[4*i + 3]);
        __m128 row = _mm_add_ps(
                    _mm_add_ps(
                        _mm_mul_ps(brod1, row1),
                        _mm_mul_ps(brod2, row2)),
                    _mm_add_ps(
                        _mm_mul_ps(brod3, row3),
                        _mm_mul_ps(brod4, row4)));
        _mm_store_ps(&C[4*i], row);
    }
}



void blockedMultiply(double * restrict A, double * restrict B, double * restrict C)
{
  struct timespec start, end;
  clock_gettime(CLOCK_MONOTONIC, &start);

  const int NB =  40;
  double sum;

  double *X = __builtin_assume_aligned(A, 16);
  double *Y = __builtin_assume_aligned(B, 16);
  double *Z = __builtin_assume_aligned(C, 16);

  for(int i=0; i<ndim; i+=NB)
    for(int j=0; j<ndim; j+=NB)
      for(int k=0; k<ndim; k+=NB)
      {
        int i_max = (i+NB < ndim)? i+NB : ndim;
        int j_max = (j+NB < ndim)? j+NB : ndim;
        int k_max = (k+NB < ndim)? k+NB : ndim;
        for(int i0=i; i0<i_max; i0++)
          for(int j0=j; j0<j_max; j0++)
            {

              sum = 0;
              for(int k0=k; k0<k_max; k0++)
                sum += A[IDX(i0,k0)] * B[IDX(k0,j0)];
              C[IDX(i0,j0)] += sum;
            }
        }


  clock_gettime(CLOCK_MONOTONIC, &end);
  float seconds = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / BILLION;

  printf("blocked took: %f\n\n", seconds);
}