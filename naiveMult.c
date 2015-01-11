// #define _POSIX_C_SOURCE >= 199309L
#include <time.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sched.h>
#include <numa.h>

#include "naiveMult.h"

// #define NUM_THREADS 1
//ndim must be dividable by NUM_THREADS
// int ndim = 64; //global

#define IDX(Y, X) (ndim * Y + X) //rows first
#define InvIDX(X, Y) (ndim * Y + X) //rows first
#define BILLION 1E9

typedef enum { false, true } bool;

typedef struct {
  int startColumn;
  int lastColumn;
  double *first;
  double *second;
  double *output;

} threadArguments;

void *multiplyPart(void *args)
{
  int k,j,i;
  struct timespec start, end;
  clock_gettime(CLOCK_THREAD_CPUTIME_ID, &start);
  threadArguments *a = (threadArguments*) args;
  // numa_run_on_node(1);


  double* first = a->first;
  double* second = a->second;
  double* output = a->output;

  for (i=a->startColumn; i <= a->lastColumn; i++) {
    for (k = 0; k < ndim; k++) {
      for (j = 0; j < ndim; j++) {
        output[IDX(i,k)] += first[IDX(i,j)] * second[IDX(j,k)];
      }
    }
  }

  clock_gettime(CLOCK_THREAD_CPUTIME_ID, &end);
  float seconds = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / BILLION;

  printf("naive parallel matrix multiplication (%d-%d) took: %f\n\n", a->startColumn, a->lastColumn, seconds);

  pthread_exit((void*) args);
}


bool isValid(double first[], double second[], double multiplied[])
{
    int i, j, k;
    double sum = 0;
    bool valid = true;
    //standard matrix multiplication (see wikipedia pseudocode)
    for (i = 0; i < ndim; i++)
    {
      for (k = 0; k < ndim; k++)
      {
        for (j = 0; j < ndim; j++)
        {
          sum = sum + first[IDX(i,j)]*second[IDX(j,k)];
        }

        if (multiplied[IDX(i,k)] != sum)
        {
          valid = false;
          printf("result matrix not valid in row %d, col %d \n", i, k);
          break;
        }

        sum = 0;
      }
    }

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
          multiply[IDX(i,k)] = sum + first[IDX(i,j)]*second[IDX(j,k)];


        // multiply[IDX(i,k)] = sum;
        // sum = 0;
      }


  clock_gettime(CLOCK_THREAD_CPUTIME_ID, &end);
  double seconds = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / BILLION;

  printf("naiveMultiplication took: %f\n\n", seconds);
}


void parallelSum(double first[], double second[], double multiply[]) {
  // basic process:
  //   divide first vertically in n parts and
  //   divide second horizontally in n parts
  //   on the ith node: calculate the i/n ... (i+1)/n sums

  int halfMatrixCellCount = ndim * ndim / 2;
  size_t halfMatrixSize = halfMatrixCellCount * sizeof(double);

  // divide matrices
  double *firstA = (double *) numa_alloc_onnode(halfMatrixSize, 0);
  double *firstB = (double *) numa_alloc_onnode(halfMatrixSize, 1);

  double *secondA = (double *) numa_alloc_onnode(halfMatrixSize, 0);
  double *secondB = (double *) numa_alloc_onnode(halfMatrixSize, 1);

  memcpy(firstA, first, halfMatrixSize);
  memcpy(firstB, first + halfMatrixCellCount, halfMatrixSize);

  memcpy(secondA, second, halfMatrixSize);
  memcpy(secondB, second + halfMatrixCellCount, halfMatrixSize);


  numa_tonode_memory((void*) firstA, halfMatrixSize, 0);
  numa_tonode_memory((void*) firstB, halfMatrixSize, 1);

  numa_tonode_memory((void*) secondA, halfMatrixSize, 0);
  numa_tonode_memory((void*) secondB, halfMatrixSize, 1);

  // TODO: transpose second matrix
  // either write transposed values to a new matrix (either local and then move or immediately remote)
  // or in-memory

  printf("1\n");

  int x, y, z;
  for (x = 0; x < ndim; x++) {
    for (y = 0; y < ndim; y++) {
      for (z = 0; z < ndim / 2; z++) {
        // compute the z-th summand for cell (y, z):
        // = (y, z) * (z, x)
        // since we transposed the first matrix, the index for the first matrix
        // has to be inverted (we use InvIDX instead of IDX)

        // TODO: recheck indices
        multiply[IDX(y, x)] += firstA[InvIDX(y, z)] * secondA[IDX(z, x)];
      }
    }
  }

  printf("2\n");
  for (x = 0; x < ndim; x++) {
    for (y = 0; y < ndim; y++) {
      for (z = ndim / 2; z < ndim; z++) {
        // if (IDX(y, x) >= halfMatrixCellCount * 2)
        //   printf("1 invalid idx: %d %d %d %d\n", IDX(y, z), x, y, z);
        // if (InvIDX(y, z) >= halfMatrixCellCount)
        //   printf("2 invalid idx: %d %d %d %d\n", InvIDX(y, z), x, y, z);
        // if (IDX(z, x) >= halfMatrixCellCount)
        //   printf("3 invalid idx: %d %d %d %d\n", IDX(z, x), x, y, z);

        multiply[IDX(y, x)] += firstB[InvIDX(y, z) - halfMatrixCellCount] * secondB[IDX(z, x) - halfMatrixCellCount];
      }
    }
  }

  printf("3\n");
  // if (isValid(first, second, multiply))
  //   printf("valid matrix multiplication\n");
  // else {
  //   printf("invalid :(\n");
  // }

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
      threadArgs[i].lastColumn = (ndim / NUM_THREADS * (i+1)) - 1;
      threadArgs[i].first = first;
      threadArgs[i].second = second;
      threadArgs[i].output = multiply;

      rc = pthread_create( &thread[i],  &attr, multiplyPart, (void*) &threadArgs[i]);
      if(rc)
      {
        fprintf(stderr,"Error - pthread_create() return code: %d\n",rc);
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