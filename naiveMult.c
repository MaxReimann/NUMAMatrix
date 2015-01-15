// #define _POSIX_C_SOURCE >= 199309L
#define _GNU_SOURCE

#include <time.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sched.h>
#include <numa.h>
#include <assert.h>
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
#define useBlocking false

typedef struct {
  int startColumn;
  int lastColumn;
  long *first;
  long *second;
  long *output;
  int currentNode;
} threadArguments;

typedef struct {
  long *partitionFirst;
  long *partitionSecond;
  long *output;
  int summandIdx;
  int currentNode;
} parallelSum_args;


void *multiplyPart(void *args)
{
  struct timespec start, end;
  clock_gettime(CLOCK_THREAD_CPUTIME_ID, &start);
  threadArguments *a = (threadArguments*) args;

  if (useNumaAdvantages) {
    numa_run_on_node(a->currentNode);
  }
  if (forceSingleNode) {
    numa_run_on_node(0);
  }

  if (useBlocking) {
    primitiveMultiply_withBlocking(a->first, a->second, a->output, a->startColumn, a->lastColumn);
  } else {
    primitiveMultiply_withoutBlocking(a->first, a->second, a->output, a->startColumn, a->lastColumn);
  }

  clock_gettime(CLOCK_THREAD_CPUTIME_ID, &end);
  float seconds = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / BILLION;

  printf("naive parallel matrix multiplication (%d-%d) took: %lu\n\n", a->startColumn, a->lastColumn, seconds);


  pthread_exit((void*) args);
}


void checkValidity(long first[], long second[], long multiplied[])
{
  if (true)
    return;

  int i, j, k;
  long sum = 0;
  bool valid = true;
  // standard matrix multiplication (see wikipedia pseudocode)
  for (i = 0; i < ndim; i++) {
    for (k = 0; k < ndim; k++) {
      for (j = 0; j < ndim; j++) {
        sum = sum + first[IDX(i,j)]*second[IDX(j,k)];
      }

      if (multiplied[IDX(i,k)] != sum) {
        valid = false;
        printf("result matrix not valid in row %d, col %d diff: %lu \n", i, k, multiplied[IDX(i,k)] - sum);
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


void primitiveMultiply_withoutBlocking(long *first, long *second, long *multiply,
  int startColumn, int lastColumn)
{

  int i, j, k;
  int sum;
  // standard matrix multiplication
  for (i = startColumn; i < lastColumn; i++) {
    for (k = 0; k < ndim; k++) {
      sum = 0;
      for (j = 0; j < ndim; j++) {
        sum += first[IDX(i, j)]*second[IDX(j, k)];
      }
      multiply[IDX(i, k)] += sum;
    }
  }
}

void primitiveMultiply_withBlocking(long *A, long *B, long *C,
  int startColumn, int lastColumn)
{
  const int NB = 40;
  long sum;

  for(int i = startColumn; i < lastColumn; i += NB) {
    for(int j = 0; j < ndim; j += NB) {
      for(int k = 0; k < ndim; k += NB) {
        int i_max = (i + NB < ndim) ? i + NB : ndim;
        int j_max = (j + NB < ndim) ? j + NB : ndim;
        int k_max = (k + NB < ndim) ? k + NB : ndim;
        for(int i0 = i; i0 < i_max; i0++) {
          for(int j0 = j; j0 < j_max; j0++) {
            sum = 0;
            for(int k0 = k; k0 < k_max; k0++)
              sum += A[IDX(i0, k0)] * B[IDX(k0, j0)];
            C[IDX(i0, j0)] += sum;
          }
        }
      }
    }
  }
}

void naiveMultiplication(long first[], long second[], long multiply[])
{
  struct timespec start, end;
  clock_gettime(CLOCK_THREAD_CPUTIME_ID, &start);

  printf("Running naiveMultiplication\n");

  primitiveMultiply_withoutBlocking(first, second, multiply, 0, ndim);

  clock_gettime(CLOCK_THREAD_CPUTIME_ID, &end);
  long seconds = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / BILLION;

  printf("naiveMultiplication took: %lu\n\n", seconds);
}

void doubleBlockedMultiply(long A[], long B[], long C[]) {
  struct timespec start, end;
  // MMM loop nest (j, i, k)

  int NB = 40;

  // ku 2 mu 10 nu 20 seconds 1.53
  // ku 4 mu 20 nu 40 seconds 1.53
  // ku 5 mu 10 nu 40 seconds 1.52
  // ku 8 mu 10 nu 40 seconds 1.50
  // ku 20 mu 10 nu 20 seconds 1.50

  // int maxKU = 500;
  // int maxMU = 500;
  // int maxNU = 500;
  long seconds;

  // int KU = 1;
  // int MU = 2;
  // int NU = 2;

  // float bestTime = 100000;
  // int bestKU, bestMU, bestNU;

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


        printf("KU: %d MU: %d NU: %d    seconds: %lu\n", KU, MU, NU, seconds);

  //       if (seconds > 3) {
  //         printf("don't check further NUs. continue...\n");
  //         goto abortingNU;
  //       }
  //     }
  //     abortingNU:;
  //   }
  // }

  // printf("best:   KU: %d MU: %d NU: %d    seconds: %lu\n", bestKU, bestMU, bestNU, seconds);



  // printf("doubleBlockedMultiply took: %lu\n\n", seconds);
}







void *parallelSum_multiplyPart(void *uncastedArgs) {
  struct timespec start, end;
  double seconds;
  clock_gettime(CLOCK_THREAD_CPUTIME_ID, &start);
  parallelSum_args *args = (parallelSum_args*) uncastedArgs;

  if (useNumaAdvantages) {
    numa_run_on_node(args->currentNode);
  }

  if (forceSingleNode) {
    numa_run_on_node(0);
  }

  int x, y, z;

  long* multiply = args->output;
  long* partitionFirst = args->partitionFirst;
  long* partitionSecond = args->partitionSecond;
  int startZ = args->summandIdx;
  int sumCount = ndim / NUM_THREADS;

  int partitionSize = ndim / NUM_NODES;
  int offset = args->currentNode * partitionSize;
  long tmpSum;

  if (useBlocking) {
    const int NB = 25;

    for (int bigY = 0; bigY < ndim; bigY += NB) {
      for (int bigX = 0; bigX < ndim; bigX += NB) {
        int x_max = (bigX + NB < ndim) ? bigX + NB : ndim;
        int y_max = (bigY + NB < ndim) ? bigY + NB : ndim;

        for (y = bigY; y < y_max; y++) {
          for (x = bigX; x < x_max; x++) {
            tmpSum = 0;
            for (z = startZ; z < startZ + sumCount; z++) {
              // compute the z-th summand for cell (y, z):
              // = (y, z) * (z, x)
              // since we transposed the first matrix, the index for the first matrix
              // has to be inverted (we use InvIDX instead of IDX)

              tmpSum += partitionFirst[IDX(z, y) - offset] * partitionSecond[InvIDX(x, z) - offset];
            }
            multiply[IDX(y, x)] += tmpSum;
          // __sync_add_and_fetch(&multiply[IDX(y, x)], tmpSum);

          }
        }
      }
    }

  } else {
    for (y = 0; y < ndim; y++) {
      for (x = 0; x < ndim; x++) {
        for (z = startZ; z < startZ + sumCount; z++) {
          tmpSum = 0;
          // compute the z-th summand for cell (y, z):
          // = (y, z) * (z, x)
          // since we transposed the first matrix, the index for the first matrix
          // has to be inverted (we use InvIDX instead of IDX)

          tmpSum += partitionFirst[IDX(z, y) - offset] * partitionSecond[InvIDX(x, z) - offset];
        }
        multiply[IDX(y, x)] += tmpSum;
        // __sync_add_and_fetch(&multiply[IDX(y, x)], tmpSum);

      }
    }
  }


  clock_gettime(CLOCK_THREAD_CPUTIME_ID, &end);
  seconds = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / BILLION;

  printf("parallelSum_multiplyPart took: %lf\n\n", seconds);
  return NULL;
}

void printMatrix(long m[]) {
  for (int y = 0; y < ndim; y++) {
    for (int x = 0; x < ndim; x++) {
      printf("%lu ", m[IDX(y, x)]);
    }
    printf("\n");
  }
}

void transposeMatrix(long *matrixToTranspose) {
  long tmp;
  for (int x = 0; x < ndim; x++) {
    for (int y = x; y < ndim; y++) {
      tmp = matrixToTranspose[IDX(y, x)];
      matrixToTranspose[IDX(y, x)] = matrixToTranspose[InvIDX(y, x)];
      matrixToTranspose[InvIDX(y, x)] = tmp;
    }
  }
}

void parallelSum(long first[], long second[], long multiply[]) {
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

  assert(((long) ndim) / NUM_THREADS - (int)(ndim / NUM_THREADS) == 0);
  assert(((long) ndim) / NUM_NODES - (int)(ndim / NUM_NODES) == 0);

  int partitionSize = ndim / NUM_NODES;


  long** firstPartitions = malloc(NUM_NODES * sizeof(long*));
  long** secondPartitions = malloc(NUM_NODES * sizeof(long*));
  long** resultPartitions = malloc(NUM_NODES * sizeof(long*));


  for (int i = 0; i < NUM_NODES; ++i) {
    firstPartitions[i] = (long *) numa_alloc_onnode(partitionSize, i);
    secondPartitions[i] = (long *) numa_alloc_onnode(partitionSize, i);
    resultPartitions[i] = (long *) numa_alloc_onnode(2 * halfMatrixSize, i);

    memcpy(firstPartitions[i], first + partitionSize * i, partitionSize);
    memcpy(secondPartitions[i], second + partitionSize * i, partitionSize);
  }

  clock_gettime(CLOCK_MONOTONIC, &end);
  seconds = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / BILLION;
  printf("preprocessing of input took: %lu\n\n", seconds);

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
    int currentNode = NUM_NODES * i / NUM_THREADS;
    threadArgs[i].summandIdx = i * sumCount;
    threadArgs[i].partitionFirst = firstPartitions[currentNode];
    threadArgs[i].partitionSecond = secondPartitions[currentNode];
    threadArgs[i].output = resultPartitions[currentNode];
    threadArgs[i].currentNode = currentNode;

    rc = pthread_create(&thread[i],  &attr, parallelSum_multiplyPart, (void*) &threadArgs[i]);
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


void parallelNaive(long first[], long second[], long multiply[])
{
  int rc;
  pthread_t thread[NUM_THREADS];
  threadArguments threadArgs[NUM_THREADS];
  pthread_attr_t attr;
  void *status;
  struct timespec start, end;
  float seconds;

  clock_gettime(CLOCK_MONOTONIC, &start);

  long** firstCopies = malloc(NUM_NODES * sizeof(long*));
  long** secondCopies = malloc(NUM_NODES * sizeof(long*));
  long** resultCopies = malloc(NUM_NODES * sizeof(long*));


  for (int i = 0; i < NUM_NODES; ++i) {
    firstCopies[i] = (long *) numa_alloc_onnode(2 * halfMatrixSize, i);
    secondCopies[i] = (long *) numa_alloc_onnode(2 * halfMatrixSize, i);
    resultCopies[i] = (long *) numa_alloc_onnode(2 * halfMatrixSize, i);

    memcpy(firstCopies[i], first, 2 * halfMatrixSize);
    memcpy(secondCopies[i], second, 2 * halfMatrixSize);
  }

  clock_gettime(CLOCK_MONOTONIC, &end);
  seconds = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / BILLION;

  printf("parallelNaive preprocessing took: %lu\n\n", seconds);

  printf("Running parallelNaive new\n");

  clock_gettime(CLOCK_MONOTONIC, &start);

  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
  for (int i = 0; i < NUM_THREADS; i++) {
    int currentNode = NUM_NODES * i / NUM_THREADS;

    threadArgs[i].startColumn = ndim / NUM_THREADS * i;
    threadArgs[i].lastColumn = ndim / NUM_THREADS * (i + 1);
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

  printf("parallelNaive took: %lu\n\n", seconds);

  checkValidity(first, second, multiply);

}