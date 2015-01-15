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
  int currentNode;
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

  if (useNumaAdvantages) {
    numa_run_on_node(a->currentNode);
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
        printf("result matrix not valid in row %d, col %d diff: %f \n", i, k, multiplied[IDX(i,k)] - sum);
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

void doubleBlockedMultiply(double A[], double B[], double C[]) {
  struct timespec start, end;
  double sum = 0;
  // MMM loop nest (j, i, k)

  int NB = 40;

  // ku 2 mu 10 nu 20 seconds 1.53
  // ku 4 mu 20 nu 40 seconds 1.53
  // ku 5 mu 10 nu 40 seconds 1.52
  // ku 8 mu 10 nu 40 seconds 1.50
  // ku 20 mu 10 nu 20 seconds 1.50

  int maxKU = 500;
  int maxMU = 500;
  int maxNU = 500;
  double seconds;

  // int KU = 1;
  // int MU = 2;
  // int NU = 2;

  float bestTime = 100000;
  int bestKU, bestMU, bestNU;

  #define KU 2
  #define MU 10
  #define NU 20

  // for(int KU = 100; KU <= maxKU; KU++) {
  //   if (ndim % KU != 0)
  //     continue;

  //   for(int MU = 2; MU <= maxMU; MU++) {
  //     if (ndim % MU != 0)
  //       continue;
  //     for(int NU = 2; NU <= maxNU; NU++) {
  //       // printf("test\n");
  //       if (ndim % NU != 0)
  //         continue;
  //       printf("  testing KU: %d MU: %d NU: %d\n", KU, MU, NU);
        clock_gettime(CLOCK_THREAD_CPUTIME_ID, &start);

        // test
        for(int i=0; i<ndim; i+=NB)
          for(int j=0; j<ndim; j+=NB)
            for(int k=0; k<ndim; k+=NB) {

              // int i_max = ( < ndim) ? i + NB : ndim;
              // int j_max = ( < ndim) ? j + NB : ndim;
              // int k_max = ( < ndim) ? k + NB : ndim;

              // mini-MMM loop nest (i0, j0, k0)
              for(int i0=i; i0 < i + NB; i0+=MU)
                for(int j0=j; j0 < j + NB; j0+=NU)
                  for(int k0=k; k0 < k + NB; k0+=KU) {
                    // micro-MMM loop nest (j00, i00)
                    for(int k00=k0; k00 < (k0 + KU); k00++)
                      for(int j00=j0; j00 < (j0 + NU); j00++)
                        for(int i00=i0; i00 < (i0 + MU); i00++)
                          C[IDX(i00, j00)]+=A[IDX(i00, k00)]*B[IDX(k00, j00)];
                  }
            }


        // printf("test2\n");
        clock_gettime(CLOCK_THREAD_CPUTIME_ID, &end);
        seconds = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / BILLION;

  //       if (seconds < bestTime) {
  //         bestTime = seconds;
  //         bestKU = KU;
  //         bestNU = NU;
  //         bestMU = MU;
  //         printf("new best time!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
  //       }


        printf("KU: %d MU: %d NU: %d    seconds: %f\n", KU, MU, NU, seconds);

  //       if (seconds > 3) {
  //         printf("don't check further NUs. continue...\n");
  //         goto abortingNU;
  //       }
  //     }
  //     abortingNU:;
  //   }
  // }

  // printf("best:   KU: %d MU: %d NU: %d    seconds: %f\n", bestKU, bestMU, bestNU, seconds);



  // printf("doubleBlockedMultiply took: %f\n\n", seconds);
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
  struct timespec start, end;
  float seconds;

  double *firstA, *firstB, *secondA, *secondB, *resultMatrixB;
  clock_gettime(CLOCK_MONOTONIC, &start);

  // if (useNumaAdvantages) {
    //
    // create copies of first, second and multiply on numa node 0 and 1
    //

    double** firstCopies = malloc(NUM_NODES * sizeof(double*));
    double** secondCopies = malloc(NUM_NODES * sizeof(double*));
    double** resultCopies = malloc(NUM_NODES * sizeof(double*));


    for (int i = 0; i < NUM_NODES; ++i) {
      firstCopies[i] = (double *) numa_alloc_onnode(2 * halfMatrixSize, i);
      secondCopies[i] = (double *) numa_alloc_onnode(2 * halfMatrixSize, i);
      resultCopies[i] = (double *) numa_alloc_onnode(2 * halfMatrixSize, i);

      memcpy(firstCopies[i], first, 2 * halfMatrixSize);
      memcpy(secondCopies[i], second, 2 * halfMatrixSize);
      memcpy(resultCopies[i], multiply, 2 * halfMatrixSize);
    }

  // } else {
  //   firstA = first;
  //   firstB = first;
  //   secondA = second;
  //   secondB = second;
  //   resultMatrixB = multiply;
  // }

  clock_gettime(CLOCK_MONOTONIC, &end);
  seconds = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / BILLION;

  printf("parallelNaive preprocessing took: %f\n\n", seconds);

  printf("Running parallelNaive new\n");

  clock_gettime(CLOCK_MONOTONIC, &start);

  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
  for (int i = 0; i < NUM_THREADS; i++) {
    int currentNode = NUM_NODES * i / NUM_THREADS;

    threadArgs[i].startColumn = ndim / NUM_THREADS * i;
    threadArgs[i].lastColumn = (ndim / NUM_THREADS * (i+1)) - 1;
    threadArgs[i].first = firstCopies[currentNode];
    threadArgs[i].second = secondCopies[currentNode];
    threadArgs[i].output = resultCopies[currentNode];
    threadArgs[i].currentNode = currentNode;


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
  seconds = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / BILLION;

  printf("parallelNaive took: %f\n\n", seconds);

  checkValidity(first, second, multiply);

}