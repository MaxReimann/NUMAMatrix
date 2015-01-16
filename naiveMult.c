// #define _POSIX_C_SOURCE >= 199309L
#include <time.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sched.h>

#include "naiveMult.h"

#define IDX(Y, X) (ndim * Y + X) //rows first
#define BILLION 1E9


typedef struct
{
    int startColumn;
    int lastColumn;
    float *first;
    float *second;
    float *output;

} threadArguments;

void *multiplyPart(void *args)
{
    int k, j, i;
    struct timespec start, end;
    clock_gettime(CLOCK_THREAD_CPUTIME_ID, &start);
    threadArguments *a = (threadArguments *) args;
    // numa_run_on_node(1);


    float *first = a->first;
    float *second = a->second;
    float *output = a->output;
    float sum;

    for (i = a->startColumn; i <= a->lastColumn; i++)
    {
        for (k = 0; k < ndim; k++)
        {
            for (sum = 0, j = 0; j < ndim; j++)
                sum += first[IDX(i, j)] * second[IDX(j, k)];
            output[IDX(i, k)] = sum;
        }
    }

    clock_gettime(CLOCK_THREAD_CPUTIME_ID, &end);
    float seconds = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / BILLION;

    printf("naive parallel matrix multiplication (%d-%d) took: %f\n\n", a->startColumn, a->lastColumn, seconds);

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
            for (j = 0; j < ndim; j++)
                sum = sum + first[IDX(i, j)] * second[IDX(j, k)];

            // /printf("%f ",sum);
            if (multiplied[IDX(i, k)] != sum)
            {
                valid = false;
                printf("result matrix not valid in row %d, col %d \n", i, k);
                printf("naive: %f strassen %f\n", sum, multiplied[IDX(i, k)]);
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
              for(int k0=k; k0<k_max; k0++)
                sum += A[IDX(i0,k0)] * B[IDX(k0,j0)];
              C[IDX(i0,j0)] += sum;
            }
        }


  clock_gettime(CLOCK_MONOTONIC, &end);
  float seconds = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / BILLION;

  printf("blocked took: %f\n\n", seconds);
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


            // multiply[IDX(i,k)] = sum;
            // sum = 0;
        }


    clock_gettime(CLOCK_THREAD_CPUTIME_ID, &end);
    float seconds = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / BILLION;

    printf("naiveMultiplication took: %f\n\n", seconds);
}


void parallelNaive(float first[], float second[], float multiply[])
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

        rc = pthread_create( &thread[i],  &attr, multiplyPart, (void *) &threadArgs[i]);
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

    printf("parallelNaive took: %f\n\n", seconds);

    // if (isValid(first, second, multiply))
    //   printf("valid matrix multiplication\n");


}