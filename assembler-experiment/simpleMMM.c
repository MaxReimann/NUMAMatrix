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

    blockedMultiply(first, second, multiply);
}

void M4x4_SSE(int n, float *restrict A, float *restrict B, float *restrict C)
{
    __m128 row1 = _mm_load_ps(&B[IDX(0, 0)]);
    __m128 row2 = _mm_load_ps(&B[IDX(1, 0)]);
    __m128 row3 = _mm_load_ps(&B[IDX(2, 0)]);
    __m128 row4 = _mm_load_ps(&B[IDX(3, 0)]);
    for (int i = 0; i < 4; i++)
    {
        //Set all four words with the same value
        __m128 cell_i1 = _mm_set1_ps(A[IDX(i, 0)]);
        __m128 cell_i2 = _mm_set1_ps(A[IDX(i, 1)]);
        __m128 cell_i3 = _mm_set1_ps(A[IDX(i, 2)]);
        __m128 cell_i4 = _mm_set1_ps(A[IDX(i, 3)]);
        //multiply i'th row of A with all columns of B
        __m128 prod1 = _mm_mul_ps(cell_i1, row1);
        __m128 prod2 = _mm_mul_ps(cell_i2, row2);
        __m128 prod3 = _mm_mul_ps(cell_i3, row3);
        __m128 prod4 = _mm_mul_ps(cell_i4, row4);
        //add up results of i'th row
        __m128 row = _mm_add_ps(
                         _mm_add_ps(prod1, prod2),
                         _mm_add_ps(prod3, prod4));
        //store result row in C
        _mm_store_ps(&C[IDX(i, 0)], row);
    }
}



void blockedMultiply(double *restrict A, double *restrict B, double *restrict C)
{
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    const int NB =  40;
    double sum;

    double *X = __builtin_assume_aligned(A, 16);
    double *Y = __builtin_assume_aligned(B, 16);
    double *Z = __builtin_assume_aligned(C, 16);

    for (int i = 0; i < ndim; i += NB)
        for (int j = 0; j < ndim; j += NB)
            for (int k = 0; k < ndim; k += NB)
            {
                int i_max = (i + NB < ndim) ? i + NB : ndim;
                int j_max = (j + NB < ndim) ? j + NB : ndim;
                int k_max = (k + NB < ndim) ? k + NB : ndim;
                for (int i0 = i; i0 < i_max; i0++)
                    for (int j0 = j; j0 < j_max; j0++)
                    {

                        sum = 0;
                        for (int k0 = k; k0 < k_max; k0++)
                            sum += A[IDX(i0, k0)] * B[IDX(k0, j0)];
                        C[IDX(i0, j0)] += sum;
                    }
            }


    clock_gettime(CLOCK_MONOTONIC, &end);
    float seconds = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / BILLION;

    printf("blocked took: %f\n\n", seconds);
}


void blockedSSEMultiply(double *restrict A, double *restrict B, double *restrict C)
{
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    const int NB =  32;
    double sum;

    double *X = __builtin_assume_aligned(A, 16);
    double *Y = __builtin_assume_aligned(B, 16);
    double *Z = __builtin_assume_aligned(C, 16);

    for (int i = 0; i < ndim; i += NB)
        for (int j = 0; j < ndim; j += NB)
            for (int k = 0; k < ndim; k += NB)
            {
                int i_max = (i + NB < ndim) ? i + NB : ndim;
                int j_max = (j + NB < ndim) ? j + NB : ndim;
                int k_max = (k + NB < ndim) ? k + NB : ndim;
                for (int i0 = i; i0 < i_max; i0++)
                    for (int j0 = j; j0 < j_max; j0++)
                    {

                        //zero out 4x4 matrix from previous use
                        __m128 zeroed = _mm_set1_ps(0.f);
                        for (int h = 0; h < 4; h++)
                            _mm_store_ps(&C[IDX((i0 + h), j0)], zeroed);

                        for (int k0 = k; k0 < k_max; k0 += 4)
                            M4x4_SSE(n, &A[IDX(i0, k0)], &B[IDX(k0, j0)], &C[IDX(i0, j0)]);
                    }
            }


    clock_gettime(CLOCK_MONOTONIC, &end);
    float seconds = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / BILLION;

    printf("blocked SSE took: %f\n\n", seconds);
}


void naiveSSEMultiplication(float A[], float B[], float C[])
{
    int i, j, k;
    struct timespec start, end;
    clock_gettime(CLOCK_THREAD_CPUTIME_ID, &start);

    printf("Running naiveMultiplication\n");

    //standard matrix multiplication (see wikipedia pseudocode)
    for (i = 0; i < ndim; i += 4)
        for (k = 0; k < ndim; k += 4)
        {
            for (j = 0; j < ndim; j += 4)
                M4x4_SSE(n, &A[IDX(i, k)], &B[IDX(k, j)], &C[IDX(i, j)]);
        }


    clock_gettime(CLOCK_THREAD_CPUTIME_ID, &end);
    float seconds = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / BILLION;

    printf("naiveMultiplication took: %f\n\n", seconds);
}

