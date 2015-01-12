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

#define useNumaAdvantages true
#define forceSingleNode false


typedef struct {
  int startColumn;
  int lastColumn;
  double *first;
  double *second;
  double *output;
  bool isFirst;
} threadArguments;

typedef struct {
  double *halfFirst;
  double *halfSecond;
  double *output;
  int summandIdx;
  bool isFirst;
} parallelSum_args;


void *multiplyPart(void *args)
{
  int k,j,i;
  struct timespec start, end;
  clock_gettime(CLOCK_THREAD_CPUTIME_ID, &start);
  threadArguments *a = (threadArguments*) args;

  if (useNumaAdvantages && !a->isFirst) {
    numa_run_on_node(1);
  }
  if (forceSingleNode) {
    numa_run_on_node(0);
  }

  double* first = a->first;
  double* second = a->second;
  double* output = a->output;
  double tmpSum;
  for (i=a->startColumn; i <= a->lastColumn; i++) {
    for (k = 0; k < ndim; k++) {
      tmpSum = 0;
      for (j = 0; j < ndim; j++) {
        tmpSum += first[IDX(i,j)] * second[IDX(j,k)];
      }
      output[IDX(i,k)] += tmpSum;
    }
  }

  clock_gettime(CLOCK_THREAD_CPUTIME_ID, &end);
  float seconds = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / BILLION;

  printf("naive parallel matrix multiplication (%d-%d) took: %f\n\n", a->startColumn, a->lastColumn, seconds);


  pthread_exit((void*) args);
}


void checkValidity(double first[], double second[], double multiplied[])
{
  if (true)
    return;

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
        printf("result matrix not valid in row %d, col %d diff: %d \n", i, k, multiplied[IDX(i,k)] - sum);
        break;
      }

      sum = 0;
    }
  }

  if (valid)
    printf("valid matrix multiplication\n");
  else {
    printf("invalid :(\n");
  }
}


void naiveMultiplication(double first[], double second[], double multiply[])
{
  int i, j, k;
  struct timespec start, end;
  double sum = 0;
  clock_gettime(CLOCK_THREAD_CPUTIME_ID, &start);

  printf("Running naiveMultiplication\n");

  // standard matrix multiplication
  for (i = 0; i < ndim; i++) {
    for (k = 0; k < ndim; k++) {
      sum = 0;
      for (j = 0; j < ndim; j++) {
        sum += first[IDX(i, j)]*second[IDX(j, k)];
      }
      multiply[IDX(i, k)] += sum;
    }
  }


  clock_gettime(CLOCK_THREAD_CPUTIME_ID, &end);
  double seconds = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / BILLION;

  printf("naiveMultiplication took: %f\n\n", seconds);
}


void parallelSum_multiplyPart(parallelSum_args *args) {
  struct timespec start, end;
  double sum = 0;
  double seconds;
  clock_gettime(CLOCK_THREAD_CPUTIME_ID, &start);

  if (useNumaAdvantages) {
    if (!args->isFirst) {
      numa_run_on_node(1);
    }
  }

  if (forceSingleNode) {
    numa_run_on_node(0);
  }

  int x, y, z;

  double* multiply = args->output;
  double* halfFirst = args->halfFirst;
  double* halfSecond = args->halfSecond;
  int startZ = args->summandIdx;
  int sumCount = ndim / NUM_THREADS;

  int offset = args->isFirst ? 0 : halfMatrixCellCount;

  double tmpSum;
  for (x = 0; x < ndim; x++) {
    for (y = 0; y < ndim; y++) {
      tmpSum = 0;
      // printf("startZ: %d\n", startZ);
      // printf("sumCount: %d\n", sumCount);
      for (z = startZ; z < startZ + sumCount; z++) {
        // compute the z-th summand for cell (y, z):
        // = (y, z) * (z, x)
        // since we transposed the first matrix, the index for the first matrix
        // has to be inverted (we use InvIDX instead of IDX)

        // if (IDX(y, x) >= halfMatrixCellCount * 2)
        //   printf("0 1 invalid idx: %d %d %d %d\n", IDX(y, z), x, y, z);
        // if (InvIDX(y, z) >= halfMatrixCellCount)
        //   printf("0 2 invalid idx: %d %d %d %d\n", InvIDX(y, z), x, y, z);
        // if (IDX(z, x) >= halfMatrixCellCount)
        //   printf("0 3 invalid idx: %d %d %d %d\n", IDX(z, x), x, y, z);
        // printf("0 startZ: %d z: %d x: %d y: %d\n", startZ, z, x, y);

        // printf("%d * %d = %d\n",
        //   halfFirst[IDX(z, y) - offset],
        //   halfSecond[InvIDX(x, z) - offset],
        //   halfFirst[IDX(z, y) - offset] * halfSecond[InvIDX(x, z) - offset]
        // );

        tmpSum += halfFirst[IDX(z, y) - offset] * halfSecond[InvIDX(x, z) - offset];
      }
      multiply[IDX(y, x)] += tmpSum;
    }
  }


  clock_gettime(CLOCK_THREAD_CPUTIME_ID, &end);
  seconds = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / BILLION;

  printf("parallelSum_multiplyPart took: %f\n\n", seconds);


}

void printMatrix(double m[]) {
  for (int y = 0; y < ndim; y++) {
    for (int x = 0; x < ndim; x++) {
      printf("%f ", m[IDX(y, x)]);
    }
    printf("\n");
  }
}

void transposeMatrix(double *matrixToTranspose) {
  double tmp;
  for (int x = 0; x < ndim; x++) {
    for (int y = x; y < ndim; y++) {
      tmp = matrixToTranspose[IDX(y, x)];
      matrixToTranspose[IDX(y, x)] = matrixToTranspose[InvIDX(y, x)];
      matrixToTranspose[InvIDX(y, x)] = tmp;
    }
  }
}

void parallelSum(double first[], double second[], double multiply[]) {
  // basic process:
  //   divide first vertically in n parts and
  //   divide second horizontally in n parts
  //   on the ith node: calculate the i/n ... (i+1)/n sums

  struct timespec start, end;
  float seconds;

  // start measuring time
  clock_gettime(CLOCK_MONOTONIC, &start);

  //
  // transpose second matrix
  //
  transposeMatrix(first);


  //
  // divide matrices and move to different nodes
  //
  double *firstA = (double *) numa_alloc_onnode(halfMatrixSize, 0);
  double *firstB = (double *) numa_alloc_onnode(halfMatrixSize, useNumaAdvantages);

  double *secondA = (double *) numa_alloc_onnode(halfMatrixSize, 0);
  double *secondB = (double *) numa_alloc_onnode(halfMatrixSize, useNumaAdvantages);

  double *resultMatrixB = (double *) numa_alloc_onnode(2 * halfMatrixSize, useNumaAdvantages);

  memcpy(firstA, first, halfMatrixSize);
  memcpy(firstB, first + halfMatrixCellCount, halfMatrixSize);

  memcpy(secondA, second, halfMatrixSize);
  memcpy(secondB, second + halfMatrixCellCount, halfMatrixSize);

  numa_tonode_memory((void*) firstA, halfMatrixSize, 0);
  numa_tonode_memory((void*) firstB, halfMatrixSize, useNumaAdvantages);

  numa_tonode_memory((void*) secondA, halfMatrixSize, 0);
  numa_tonode_memory((void*) secondB, halfMatrixSize, useNumaAdvantages);


  clock_gettime(CLOCK_MONOTONIC, &end);
  seconds = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / BILLION;
  printf("preprocessing of input took: %f\n\n", seconds);

  //
  // set up threads
  //
  int rc;
  pthread_t thread[NUM_THREADS];
  parallelSum_args threadArgs[NUM_THREADS];
  pthread_attr_t attr;
  void *status;

  printf("Running parallelSum\n");

  clock_gettime(CLOCK_MONOTONIC, &start);

  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

  int parallel = true;

  int sumCount = ndim / NUM_THREADS;
  for (int i = 0; i < NUM_THREADS; i++) {
    int firstHalf = i < NUM_THREADS / 2;
    threadArgs[i].summandIdx = i * sumCount;
    threadArgs[i].halfFirst = firstHalf ? firstA : firstB;
    threadArgs[i].halfSecond = firstHalf ? secondA : secondB;
    threadArgs[i].output = (firstHalf || true) ? multiply : resultMatrixB;
    threadArgs[i].isFirst = firstHalf;

    rc = pthread_create( &thread[i],  &attr, parallelSum_multiplyPart, (void*) &threadArgs[i]);
    if(rc) {
      fprintf(stderr,"Error - pthread_create() return code: %d\n",rc);
      exit(EXIT_FAILURE);
    }
    if (!parallel) {
      pthread_attr_destroy(&attr);
      pthread_join(thread[i], &status);
    }
  }


  /* Free attribute and wait for the other threads */
  if (parallel) {
    pthread_attr_destroy(&attr);
    for (int i = 0; i < NUM_THREADS; i++) {
      rc = pthread_join(thread[i], &status);
      if (rc) {
         printf("ERROR; return code from pthread_join() is %d\n", rc);
         exit(EXIT_FAILURE);
      }
    }
  }


  clock_gettime(CLOCK_MONOTONIC, &end);
  seconds = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / BILLION;
  printf("parallelSum took: %f\n\n", seconds);

  transposeMatrix(first);

  checkValidity(first, second, multiply);
}


void parallelNaive(double first[], double second[], double multiply[])
{
  int rc;
  pthread_t thread[NUM_THREADS];
  threadArguments threadArgs[NUM_THREADS];
  pthread_attr_t attr;
  void *status;

  double *firstA, *firstB, *secondA, *secondB, *resultMatrixB;

  if (useNumaAdvantages) {
    //
    // create copies of first, second and multiply on numa node 0 and 1
    //

    firstA = (double *) numa_alloc_onnode(2 * halfMatrixSize, 0);
    firstB = (double *) numa_alloc_onnode(2 * halfMatrixSize, 1);

    secondA = (double *) numa_alloc_onnode(2 * halfMatrixSize, 0);
    secondB = (double *) numa_alloc_onnode(2 * halfMatrixSize, 1);

    resultMatrixB = (double *) numa_alloc_onnode(2 * halfMatrixSize, 1);


    memcpy(firstA, first, 2 * halfMatrixSize);
    memcpy(firstB, first, 2 * halfMatrixSize);

    memcpy(secondA, second, 2 * halfMatrixSize);
    memcpy(secondB, second, 2 * halfMatrixSize);

    numa_tonode_memory((void*) firstA, halfMatrixSize, 0);
    numa_tonode_memory((void*) firstB, halfMatrixSize, 1);

    numa_tonode_memory((void*) secondA, halfMatrixSize, 0);
    numa_tonode_memory((void*) secondB, halfMatrixSize, 1);
  } else {
    firstA = first;
    firstB = first;
    secondA = second;
    secondB = second;
    resultMatrixB = multiply;
  }



  printf("Running parallelNaive\n");

  struct timespec start, end;
  clock_gettime(CLOCK_MONOTONIC, &start);

  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
  for (int i = 0; i < NUM_THREADS; i++) {
    int firstHalf = i < NUM_THREADS / 2;
    threadArgs[i].startColumn = ndim / NUM_THREADS * i;
    threadArgs[i].lastColumn = (ndim / NUM_THREADS * (i+1)) - 1;
    threadArgs[i].first = firstHalf ? firstA : firstB;
    threadArgs[i].second = firstHalf ? secondA : secondB;
    threadArgs[i].output = firstHalf ? multiply : resultMatrixB;
    threadArgs[i].isFirst = firstHalf;


    rc = pthread_create( &thread[i],  &attr, multiplyPart, (void*) &threadArgs[i]);
    if(rc) {
      fprintf(stderr,"Error - pthread_create() return code: %d\n",rc);
      exit(EXIT_FAILURE);
    }
    // rc = pthread_join(thread[i], &status);
  }


  /* Free attribute and wait for the other threads */
  pthread_attr_destroy(&attr);
  for (int i = 0; i < NUM_THREADS; i++) {
    rc = pthread_join(thread[i], &status);
    if (rc) {
       printf("ERROR; return code from pthread_join() is %d\n", rc);
       exit(EXIT_FAILURE);
    }
  }


  clock_gettime(CLOCK_MONOTONIC, &end);
  float seconds = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / BILLION;

  printf("parallelNaive took: %f\n\n", seconds);

  checkValidity(first, second, multiply);

}