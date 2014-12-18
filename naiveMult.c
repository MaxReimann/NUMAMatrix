#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <pthread.h>

#include "naiveMult.h"

#define NUM_THREADS 4
//ndim must be devidable by NUM_THREADS
int ndim = 512; //global

#define IDX(Y, X) (ndim * Y + X) //rows first


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
    threadArguments *a = (threadArguments*) args;
    
    for (i=a->startColumn; i <= a->lastColumn; i++)
    {
      for (k = 0; k < ndim; k++)
      {
        for (j = 0; j < ndim; j++)
          a->output[IDX(i,k)] += a->first[IDX(i,j)] * a->second[IDX(j,k)];
      }
    }

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


void naiveMultiplication(int n, double first[], double second[], double multiply[])
{
  int i, j, k;
  double sum = 0;
  ndim = n;
  //standard matrix multiplication (see wikipedia pseudocode) 
  for (i = 0; i < ndim; i++)
      for (k = 0; k < ndim; k++)
      {
        for (j = 0; j < ndim; j++)
          sum = sum + first[IDX(i,j)]*second[IDX(j,k)];
       
        multiply[IDX(i,k)] = sum;
        sum = 0;
      }
}


void parallelNaive(int n, double first[], double second[], double multiply[])
{
  int c, d, k, sI, rc, i;
  pthread_t thread[NUM_THREADS];
  threadArguments threadArgs[NUM_THREADS];
  pthread_attr_t attr;
  void *status;

  ndim = n;

  for (c = 0 ; c < ndim; c++)
  {
    for (d = 0 ; d < ndim; d++)
    {
      first[IDX(c,d)] = rand() % 100; //int between 0 - 1000
      second[IDX(c,d)] = rand() % 100;
      multiply[IDX(c,d)] = 0;
    }
  
  }
  clock_t start = clock();

  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

  for (i = 0; i < NUM_THREADS; i++)
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
  for (i = 0; i < NUM_THREADS; i++)
  {
    rc = pthread_join(thread[i], &status);
    if (rc) 
    {
       printf("ERROR; return code from pthread_join() is %d\n", rc);
       exit(EXIT_FAILURE);
    }
  }

  clock_t end = clock();
  float seconds = (float)(end - start) / CLOCKS_PER_SEC;

  printf("parallel matrix multiplication took: %f\n", seconds);

  if (isValid(first, second, multiply))
    printf("valid matrix multiplication\n");


}