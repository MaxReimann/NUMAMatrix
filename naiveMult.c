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
    double *first;
    double *second;
    double *output;

} threadArguments;

void *multiplyPart(void *args)
{
    int k, j, i;
    struct timespec start, end;
    clock_gettime(CLOCK_THREAD_CPUTIME_ID, &start);
    threadArguments *a = (threadArguments *) args;
    // numa_run_on_node(1);


    double *first = a->first;
    double *second = a->second;
    double *output = a->output;
    double sum;

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


bool isValid(double first[], double second[], double multiplied[])
{
    int i, j, k;
    double sum = 0;
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


void naiveMultiplication(double first[], double second[], double multiply[])
{
    int i, j, k;
    struct timespec start, end;
    double sum = 0;
    clock_gettime(CLOCK_THREAD_CPUTIME_ID, &start);

    printf("Running naiveMultiplication\n");

    //standard matrix multiplication (see wikipedia pseudocode)
    for (i = 0; i < ndim; i++)
        for (k = 0; k < ndim; k++)
        {
            for (j = 0; j < ndim; j++)
                // sum = sum + first[IDX(i,j)]*second[IDX(j,k)];
                multiply[IDX(i, k)] = sum + first[IDX(i, j)] * second[IDX(j, k)];


            // multiply[IDX(i,k)] = sum;
            // sum = 0;
        }


    clock_gettime(CLOCK_THREAD_CPUTIME_ID, &end);
    double seconds = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / BILLION;

    printf("naiveMultiplication took: %f\n\n", seconds);
}


void parallelNaive(double first[], double second[], double multiply[])
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