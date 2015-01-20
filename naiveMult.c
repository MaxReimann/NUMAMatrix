// #define _POSIX_C_SOURCE >= 199309L
#include <time.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sched.h>
#include <xmmintrin.h>

#include "naiveMult.h"

#define IDX(Y, X) (ndim * Y + X) //rows first
#define BILLION 1E9

inline void M4x4_SSE(float * restrict A, float * restrict B, float * restrict C);
typedef struct
{
    int startColumn;
    int lastColumn;
    float *first;
    float *second;
    float *output;

} threadArguments;

void *multiplyPartSSE(void *args)
{
    int k, j, i;
    threadArguments *a = (threadArguments *) args;
    // numa_run_on_node(1);
    


    float *A = a->first;
    float *B = a->second;
    float *C = a->output;

    for (i = a->startColumn; i <= a->lastColumn; i+=4)
        for (k = 0; k < ndim; k+=4)
        {
            for (j = 0; j < ndim; j+=4) 
              M4x4_SSE(&A[IDX(i, k)], &B[IDX(k, j)], &C[IDX(i, j)]);
        }


    pthread_exit((void *) args);
}

void *multiplyPartBlocked(void *args)
{
    int k, j, i;
    threadArguments *a = (threadArguments *) args;
    // numa_run_on_node(1);


    float *A = a->first;
    float *B = a->second;
    float *C = a->output;

    const int NB = 32;
    float sum;

  for(int i = a->startColumn; i <= a->lastColumn;i+=NB)
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
              for(int k0=k; k0<k_max; k0++)//transposed!!
                sum += A[IDX(i0,k0)] * B[IDX(j0,k0)];
              C[IDX(i0,j0)] += sum;
            }
        }

    pthread_exit((void *) args);
}


bool isValid(float first[], float second[], float multiplied[])
{
    int i, j, k;
    float sum = 0;
    bool valid = true;
    printf("\n");
    //standard matrix multiplication (see wikipedia pseudocode)
    for (i = 0; i < ndim; i++)
    {
        for (k = 0; k < ndim; k++)
        {
            sum = 0;
            for (j = 0; j < ndim; j++)
                sum += first[IDX(i, j)] * second[IDX(j, k)];

            // /printf("%f ",sum);
            if (multiplied[IDX(i, k)] != sum)
            {
                valid = false;
                printf("result matrix not valid in row %d, col %d \n", i, k);
                printf("naive: %f strassen %f\n", sum, multiplied[IDX(i, k)]);
                break;
            }

        }
        // printf("\n");
    }
    // printf("\n");

    printf("valid matrix multiplication\n");

    return valid;
}

void blockedMultiply(float A[], float B[], float C[])
{
  struct timespec start, end;
  clock_gettime(CLOCK_MONOTONIC, &start);


  const int NB =  40;

  float sum;

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
              for(int k0=k; k0<k_max; k0++)//transposed!!
                sum += A[IDX(i0,k0)] * B[IDX(k0,j0)];
              C[IDX(i0,j0)] += sum;
            }
        }


  clock_gettime(CLOCK_MONOTONIC, &end);
  float seconds = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / BILLION;

  printf("blocked transpoed  took: %f\n\n", seconds);
}



inline void M4x4_SSE(float * restrict A, float * restrict B, float * restrict C) {
    __m128 row1 = _mm_load_ps(&B[IDX(0,0)]);
    __m128 row2 = _mm_load_ps(&B[IDX(1,0)]);
    __m128 row3 = _mm_load_ps(&B[IDX(2,0)]);
    __m128 row4 = _mm_load_ps(&B[IDX(3,0)]);
    for(int i=0; i<4; i++) {
        __m128 brod1 = _mm_set1_ps(A[IDX(i,0)]); //Set all four words with the same value 
        __m128 brod2 = _mm_set1_ps(A[IDX(i,1)]);
        __m128 brod3 = _mm_set1_ps(A[IDX(i,2)]);
        __m128 brod4 = _mm_set1_ps(A[IDX(i,3)]);


      
      __m128 prod1 = _mm_mul_ps(brod1, row1);
      __m128 prod2 = _mm_mul_ps(brod2, row2);
      __m128 prod3 = _mm_mul_ps(brod3, row3);
      __m128 prod4 = _mm_mul_ps(brod4, row4);


        __m128 row = _mm_add_ps(
                    _mm_add_ps(prod1, prod2),
          _mm_add_ps(prod3, prod4));

         __m128 res = _mm_load_ps(&C[IDX(i,0)]);
         res = _mm_add_ps(res, row);
        _mm_store_ps(&C[IDX(i,0)], res);
    }
}



void blockedSSEMultiply(float *restrict A, float *restrict B, float *restrict C, int blockSize)
{
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    int NB =  128;
    float sum;

    for (int i = 0; i < ndim; i += NB)
        for (int j = 0; j < ndim; j += NB)
            for (int k = 0; k < ndim; k += NB)
            {
                int i_max = (i + NB < ndim) ? i + NB : ndim;
                int j_max = (j + NB < ndim) ? j + NB : ndim;
                int k_max = (k + NB < ndim) ? k + NB : ndim;
                for (int i0 = i; i0 < i_max; i0+=4)
                    for (int j0 = j; j0 < j_max; j0+=4)
                    {

                        //zero out 4x4 matrix from previous use
                        //#__m128 zeroed = _mm_set1_ps(0.f);
                        //for (int h = 0; h < 4; h++)
                        //    _mm_store_ps(&C[IDX((i0 + h), j0)], zeroed);

                        for (int k0 = k; k0 < k_max; k0 += 4)
                            M4x4_SSE(&A[IDX(i0, k0)], &B[IDX(k0, j0)], &C[IDX(i0, j0)]);
                    }
            }


    clock_gettime(CLOCK_MONOTONIC, &end);
    float seconds = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / BILLION;

    printf("blocked  SSE took: %f\n\n", seconds);
}


void naiveSSEMultiplication(float A[], float B[], float C[])
{
    int i, j, k;
    struct timespec start, end;
    float sum = 0;
    clock_gettime(CLOCK_THREAD_CPUTIME_ID, &start);

    //standard matrix multiplication (see wikipedia pseudocode)
    for (i = 0; i < ndim; i+=4)
        for (k = 0; k < ndim; k+=4)
        {
            for (j = 0; j < ndim; j+=4) 
              M4x4_SSE(&A[IDX(i, k)], &B[IDX(k, j)], &C[IDX(i, j)]);
        }


    clock_gettime(CLOCK_THREAD_CPUTIME_ID, &end);
    float seconds = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / BILLION;

    printf("naiveSSEMultiplication took: %f\n\n", seconds);
}























void naiveMultiplication(float first[], float second[], float multiply[])
{
    int i, j, k;
    struct timespec start, end;
    float sum = 0;
    clock_gettime(CLOCK_THREAD_CPUTIME_ID, &start);

    printf("Running naiveMultiplication\n");

    //standard matrix multiplication (see wikipedia pseudocode)
    for (i = 0; i < ndim; i++)
        for (k = 0; k < ndim; k++)
        {
            for (j = 0; j < ndim; j++)
                 sum = sum + first[IDX(i,j)]*second[IDX(j,k)];
            
            multiply[IDX(i, k)] = sum;
            sum = 0;
        }


    clock_gettime(CLOCK_THREAD_CPUTIME_ID, &end);
    float seconds = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / BILLION;

    printf("naiveMultiplication took: %f\n\n", seconds);
}


void parallelNaiveSSE(float first[], float second[], float multiply[])
{
    int rc;
    pthread_t thread[NUM_THREADS];
    threadArguments threadArgs[NUM_THREADS];
    pthread_attr_t attr;
    void *status;

    printf("Running parallelNaive\n");

    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    for (int i = 0; i < NUM_THREADS; i++)
    {
        threadArgs[i].startColumn = ndim / NUM_THREADS * i;
        threadArgs[i].lastColumn = (ndim / NUM_THREADS * (i + 1)) - 1;
        threadArgs[i].first = first;
        threadArgs[i].second = second;
        threadArgs[i].output = multiply;

        rc = pthread_create( &thread[i],  &attr, multiplyPartSSE, (void *) &threadArgs[i]);
        
        if (rc)
        {
            fprintf(stderr, "Error - pthread_create() return code: %d\n", rc);
            exit(EXIT_FAILURE);
        }
    }


    /* Free attribute and wait for the other threads */
    pthread_attr_destroy(&attr);
    for (int i = 0; i < NUM_THREADS; i++)
    {
        rc = pthread_join(thread[i], &status);
        if (rc)
        {
            printf("ERROR; return code from pthread_join() is %d\n", rc);
            exit(EXIT_FAILURE);
        }
    }


    clock_gettime(CLOCK_MONOTONIC, &end);
    float seconds = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / BILLION;

    printf("parallelNaiveSSE took: %f\n\n", seconds);

    // if (isValid(first, second, multiply))
    //   printf("valid matrix multiplication\n");


}



void parallelNaiveBlocked(float first[], float second[], float multiply[])
{
    int rc;
    pthread_t thread[NUM_THREADS];
    threadArguments threadArgs[NUM_THREADS];
    pthread_attr_t attr;
    void *status;

    printf("Running parallelNaive\n");

    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    for (int i = 0; i < NUM_THREADS; i++)
    {
        threadArgs[i].startColumn = ndim / NUM_THREADS * i;
        threadArgs[i].lastColumn = (ndim / NUM_THREADS * (i + 1)) - 1;
        threadArgs[i].first = first;
        threadArgs[i].second = second;
        threadArgs[i].output = multiply;

        rc = pthread_create( &thread[i],  &attr, multiplyPartBlocked, (void *) &threadArgs[i]);
        
        if (rc)
        {
            fprintf(stderr, "Error - pthread_create() return code: %d\n", rc);
            exit(EXIT_FAILURE);
        }
    }


    /* Free attribute and wait for the other threads */
    pthread_attr_destroy(&attr);
    for (int i = 0; i < NUM_THREADS; i++)
    {
        rc = pthread_join(thread[i], &status);
        if (rc)
        {
            printf("ERROR; return code from pthread_join() is %d\n", rc);
            exit(EXIT_FAILURE);
        }
    }


    clock_gettime(CLOCK_MONOTONIC, &end);
    float seconds = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / BILLION;

    printf("parallelNaiveBlocked took: %f\n\n", seconds);

    // if (isValid(first, second, multiply))
    //   printf("valid matrix multiplication\n");


}