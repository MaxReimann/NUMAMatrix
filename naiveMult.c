// #define _POSIX_C_SOURCE >= 199309L
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

// #define TRANSPOSED

#define IDX(Y, X) (ndim * Y + X) // rows first
#define InvIDX(X, Y) (ndim * Y + X) // rows first

#ifdef TRANSPOSED
  #define tIDX(X, Y) (ndim * Y + X)
#endif

#ifndef TRANSPOSED
  #define tIDX(Y, X) (ndim * Y + X)
#endif


#define BILLION 1E9


#define useNumaAdvantages true
#define pinThreads true
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
  double *partitionFirst;
  double *partitionSecond;
  double *output;
  int summandIdx;
  int currentNode;
} parallelSum_args;


void *multiplyPart(void *args)
{
  struct timespec start, end;
  clock_gettime(CLOCK_THREAD_CPUTIME_ID, &start);
  threadArguments *a = (threadArguments*) args;

  if (pinThreads) {
    printf("running on node: %d\n", a->currentNode);
    numa_run_on_node(a->currentNode);
  }
  if (forceSingleNode) {
    printf("force to run on zero node\n");
    numa_run_on_node(0);
  }

  if (useBlocking) {
    primitiveMultiply_withBlocking(a->first, a->second, a->output, a->startColumn, a->lastColumn);
  } else {
    primitiveMultiply_withoutBlocking(a->first, a->second, a->output, a->startColumn, a->lastColumn);
  }

  clock_gettime(CLOCK_THREAD_CPUTIME_ID, &end);
  float seconds = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / BILLION;

  // printf("naive parallel matrix multiplication (%d-%d) took: %f\n\n", a->startColumn, a->lastColumn, seconds);


  pthread_exit((void*) args);
}


void checkValidity(double first[], double second[], double multiplied[])
{
  if (true)
    return;

  int i, j, k;
  double sum = 0;
  bool valid = true;
  // standard matrix multiplication (see wikipedia pseudocode)
  for (i = 0; i < ndim; i++) {
    for (k = 0; k < ndim; k++) {
      for (j = 0; j < ndim; j++) {
        sum = sum + first[IDX(i,j)]*second[IDX(j,k)];
      }

      if (multiplied[IDX(i,k)] != sum) {
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


void primitiveMultiply_withoutBlocking(double *first, double *second, double *multiply,
  int startColumn, int lastColumn)
{

  int i, j, k;
  int sum;
  // standard matrix multiplication
  for (i = startColumn; i < lastColumn; i++) {
    for (k = 0; k < ndim; k++) {
      sum = 0;
      for (j = 0; j < ndim; j++) {
        // if matrixB is transposed use InvIDX
        sum += first[IDX(i, j)] * second[tIDX(j, k)];
      }
      multiply[IDX(i, k)] += sum;
    }
  }
}

void primitiveMultiply_withBlocking(double *A, double *B, double *C,
  int startColumn, int lastColumn)
{
  const int NB = 40;
  double sum;

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
              // if matrixB is transposed use InvIDX
              sum += A[IDX(i0, k0)] * B[tIDX(k0, j0)];
            C[IDX(i0, j0)] += sum;
          }
        }
      }
    }
  }
}

void naiveMultiplication(double first[], double second[], double multiply[])
{
  struct timespec start, end;
  clock_gettime(CLOCK_THREAD_CPUTIME_ID, &start);

  printf("Running naiveMultiplication\n");

  if (useBlocking) {
    primitiveMultiply_withBlocking(first, second, multiply, 0, ndim);
  } else {
    primitiveMultiply_withoutBlocking(first, second, multiply, 0, ndim);
  }

  clock_gettime(CLOCK_THREAD_CPUTIME_ID, &end);
  double seconds = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / BILLION;

  printf("naiveMultiplication took: %f\n\n", seconds);
}

void doubleBlockedMultiply(double A[], double B[], double C[]) {
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
  double seconds;

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







void *parallelSum_multiplyPart(void *uncastedArgs) {
  struct timespec start, end;
  double seconds;
  clock_gettime(CLOCK_THREAD_CPUTIME_ID, &start);
  parallelSum_args *args = (parallelSum_args*) uncastedArgs;

  if (useNumaAdvantages || pinThreads) {
    printf("running on node: %d\n", args->currentNode);
    numa_run_on_node(args->currentNode);
  }

  if (forceSingleNode) {
    printf("force to run on 0. node\n");
    numa_run_on_node(0);
  }

  int x, y, z;

  double* multiply = args->output;
  double* partitionFirst = args->partitionFirst;
  double* partitionSecond = args->partitionSecond;
  int startZ = args->summandIdx;
  int sumCount = ndim / NUM_THREADS;

  int partitionSize = ndim * ndim / NUM_NODES;
  int offset = args->currentNode * partitionSize;
  double tmpSum;

  // printf("startZ: %d\n", startZ);
  // printf("sumCount: %d\n", sumCount);

  if (useBlocking) {
    const int NB = 40;

    for (int bigY = 0; bigY < ndim; bigY += NB) {
      // printf("b %d\n", bigY);
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

              // if (IDX(z, y) - offset >= partitionSize)
              //   printf("f %d %d %d\n", z, y, x);

              // if (InvIDX(x, z) - offset >= partitionSize)
              //   printf("ff %d %d %d\n", z, y, x);

              tmpSum += partitionFirst[IDX(z, y) - offset] * partitionSecond[InvIDX(x, z) - offset];
            }
            // if (IDX(y, x) >= ndim * ndim) {
            //   printf("fff %d %d\n", y, x);
            // }
            multiply[IDX(y, x)] += tmpSum;
            // multiply[0] += tmpSum;
            // printf("tmpSum: %f\n", tmpSum);
            // multiply[0] = tmpSum;
          }
        }
      }
    }

  } else {
    for (y = 0; y < ndim; y++) {
      // printf("%d\n", y);
      for (x = 0; x < ndim; x++) {
        for (z = startZ; z < startZ + sumCount; z++) {
          tmpSum = 0;
          // compute the z-th summand for cell (y, z):
          // = (y, z) * (z, x)
          // since we transposed the first matrix, the index for the first matrix
          // has to be inverted (we use InvIDX instead of IDX)
          // if (IDX(z, y) - offset >= partitionSize) {
          //   printf("1 %d %d %d\n", IDX(z, y) - offset, IDX(z, y), offset);

          // }

          // if (InvIDX(x, z) - offset >= partitionSize) {
          //   printf("2 %d %d %d\n", InvIDX(x, z) - offset, InvIDX(x, z), offset);
          // }



          tmpSum += partitionFirst[IDX(z, y) - offset] * partitionSecond[InvIDX(x, z) - offset];
          // tmpSum += partitionSecond[InvIDX(x, z) - offset];
        }

        // if (IDX(y, x) >= partitionSize * NUM_NODES) {
        //   printf("3 %d %d \n", y, x);
        // }
        // multiply[0] += tmpSum;
        multiply[IDX(y, x)] += tmpSum;
        // __sync_add_and_fetch(&multiply[IDX(y, x)], tmpSum);

      }
    }
  }


  clock_gettime(CLOCK_THREAD_CPUTIME_ID, &end);
  seconds = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / BILLION;

  // printf("parallelSum_multiplyPart took: %f\n\n", seconds);
  return NULL;
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

  assert(((double) ndim) / NUM_THREADS - (int)(ndim / NUM_THREADS) == 0);
  assert(((double) ndim) / NUM_NODES - (int)(ndim / NUM_NODES) == 0);

  int partitionSize = ndim * ndim / NUM_NODES;


  double** firstPartitions = malloc(NUM_NODES * sizeof(double*));
  double** secondPartitions = malloc(NUM_NODES * sizeof(double*));
  double** resultPartitions = malloc(NUM_NODES * sizeof(double*));

  int j;
  for (int i = 0; i < NUM_NODES; ++i) {
    firstPartitions[i] = (double *) numa_alloc_onnode(partitionSize, i);
    secondPartitions[i] = (double *) numa_alloc_onnode(partitionSize, i);
    resultPartitions[i] = (double *) numa_alloc_onnode(2 * halfMatrixSize, i);

    memcpy(firstPartitions[i], first + partitionSize * i, partitionSize);
    memcpy(secondPartitions[i], second + partitionSize * i, partitionSize);
  }

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
    int currentNode = NUM_NODES * i / NUM_THREADS;
    threadArgs[i].summandIdx = i * sumCount;

    if (useNumaAdvantages) {
      threadArgs[i].partitionFirst = first + partitionSize * currentNode;
      threadArgs[i].partitionSecond = second + partitionSize * currentNode;
      threadArgs[i].output = multiply;
    } else {
      threadArgs[i].partitionFirst = firstPartitions[currentNode];
      threadArgs[i].partitionSecond = secondPartitions[currentNode];
      threadArgs[i].output = resultPartitions[currentNode];
    }

    threadArgs[i].currentNode = currentNode;


    rc = pthread_create(&thread[i], &attr, parallelSum_multiplyPart, (void*) &threadArgs[i]);
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

  clock_gettime(CLOCK_MONOTONIC, &start);

  double** firstCopies = malloc(NUM_NODES * sizeof(double*));
  double** secondCopies = malloc(NUM_NODES * sizeof(double*));
  double** resultCopies = malloc(NUM_NODES * sizeof(double*));


  for (int i = 0; i < NUM_NODES; ++i) {
    firstCopies[i] = (double *) numa_alloc_onnode(2 * halfMatrixSize, i);
    secondCopies[i] = (double *) numa_alloc_onnode(2 * halfMatrixSize, i);
    resultCopies[i] = (double *) numa_alloc_onnode(2 * halfMatrixSize, i);

    memcpy(firstCopies[i], first, 2 * halfMatrixSize);
    memcpy(secondCopies[i], second, 2 * halfMatrixSize);
  }

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
    threadArgs[i].lastColumn = ndim / NUM_THREADS * (i + 1);
    threadArgs[i].first = firstCopies[currentNode];
    threadArgs[i].second = secondCopies[currentNode];
    threadArgs[i].output = resultCopies[currentNode];
    threadArgs[i].currentNode = currentNode;

    if (useNumaAdvantages) {
      threadArgs[i].first = firstCopies[currentNode];
      threadArgs[i].second = secondCopies[currentNode];
      threadArgs[i].output = resultCopies[currentNode];
    } else {
      threadArgs[i].first = first;
      threadArgs[i].second = second;
      threadArgs[i].output = multiply;
    }

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