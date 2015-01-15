#include <xmmintrin.h>
#include <stdio.h>
#include <time.h>
#include <time.h>

#include <mm_malloc.h>

#include <stdlib.h>
#define BILLION 1E9
#define IDX(Y, X) (ndim * Y + X) //rows first

const int ndim = 2000;

void blockedMultiply(float A[], float B[], float C[]);
int isValid(float first[], float second[], float multiplied[]);

int main(int argc, char **argv)
{


    srand(1);
    float *first = _mm_malloc(ndim * ndim * sizeof(float), 16);
    float *second = _mm_malloc(ndim * ndim * sizeof(float), 16);
    float *multiply = _mm_malloc(ndim * ndim * sizeof(float), 16);
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
    isValid(first,second,multiply);
}


void inline M4x4_SSE(float *A, float *B, float *C) {
    __m128 row1 = _mm_load_ps(&B[IDX(0,0)]);
    __m128 row2 = _mm_load_ps(&B[IDX(1,0)]);
    __m128 row3 = _mm_load_ps(&B[IDX(2,0)]);
    __m128 row4 = _mm_load_ps(&B[IDX(3,0)]);
    for(int i=0; i<4; i++) {
        __m128 brod1 = _mm_set1_ps(A[IDX(i,0)]); //Set all four words with the same value 
        __m128 brod2 = _mm_set1_ps(A[IDX(i,1)]);
        __m128 brod3 = _mm_set1_ps(A[IDX(i,2)]);
        __m128 brod4 = _mm_set1_ps(A[IDX(i,3)]);
        __m128 row = _mm_add_ps(
                    _mm_add_ps(
                        _mm_mul_ps(brod1, row1),
                        _mm_mul_ps(brod2, row2)),
                    _mm_add_ps(
                        _mm_mul_ps(brod3, row3),
                        _mm_mul_ps(brod4, row4)));
        __m128 res = _mm_load_ps(&C[IDX(i,0)]);
        res = _mm_add_ps(res, row);
        _mm_store_ps(&C[IDX(i,0)], res);
    }
}



void blockedMultiply(float * restrict A, float * restrict B, float * restrict C)
{
  struct timespec start, end;
  clock_gettime(CLOCK_MONOTONIC, &start);

  const int NB =  40;
  float sum;

  // float *X = __builtin_assume_aligned(A, 16);
  // float *Y = __builtin_assume_aligned(B, 16);
  // float *Z = __builtin_assume_aligned(C, 16);

  for(int i=0; i<ndim; i+=NB)
    for(int j=0; j<ndim; j+=NB)
      for(int k=0; k<ndim; k+=NB)
      {
        int i_max = (i+NB < ndim)? i+NB : ndim;
        int j_max = (j+NB < ndim)? j+NB : ndim;
        int k_max = (k+NB < ndim)? k+NB : ndim;
        for(int i0=i; i0<i_max; i0+=4)
          for(int j0=j; j0<j_max; j0+=4)
            {

              for(int k0=k; k0<k_max; k0+=4)
              {
                M4x4_SSE(&A[IDX(i0,k0)], &B[IDX(k0,j0)], &C[IDX(i0,j0)]);
              }
            }
        }


  clock_gettime(CLOCK_MONOTONIC, &end);
  float seconds = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / BILLION;

  printf("blocked took: %f\n\n", seconds);
}


int isValid(float first[], float second[], float multiplied[])
{
    int i, j, k;
    float sum = 0;
    int valid = 0;
    printf("\n");
    //standard matrix multiplication (see wikipedia pseudocode)
    for (i = 0; i < ndim; i++)
    {
        for (k = 0; k < ndim; k++)
        {
            for (j = 0; j < ndim; j++)
                sum = sum + first[IDX(i, j)] * second[IDX(j, k)];

            // /printf("%f ",sum);
            if (multiplied[IDX(i, k)] != sum)
            {
                valid = -1;
                printf("result matrix not valid in row %d, col %d \n", i, k);
                printf("naive: %f sse %f\n", sum, multiplied[IDX(i, k)]);
                break;
            }

            sum = 0;
        }
        // printf("\n");
    }
    // printf("\n");

    printf("valid matrix multiplication\n");

    return valid;
}