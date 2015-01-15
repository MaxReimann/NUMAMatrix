#include <stdio.h>
#include <time.h>
#include <time.h>
#include <sys/time.h>
#include <stdlib.h>
#include <string.h>
#include "naiveMult.h"
#include "strassenutil.h"

#include <stdint.h>
#include <sched.h>
#include <numa.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>
#include <assert.h>
#include "util.h"

#include <pthread.h>

#include "membench.h"

typedef struct {
  int from;
  int to;
} threadArguments;

// #define ndim  1024
#define IDX(Y, X) (ndim * Y + X) //rows first

int main(int argc, char **argv)
{
	NUM_THREADS = 128;
	ndim = 8192;
	NUM_NODES = 8;

	halfMatrixCellCount = (ndim * ndim / 2);
	halfMatrixSize = halfMatrixCellCount * sizeof(double);

	// srand(time(NULL));
	srand(0);

	printf("NUM_THREADS: %d\n", NUM_THREADS);
	printf("ndim: %d\n", ndim);
	setbuf(stdout, NULL);

	printf("numa node count %d\n", numa_max_node() + 1);

	int destnode = 0;
	long freep;
	numa_node_size(0, &freep);
	printf("freep in GB on 0: %f\n", (float) freep / 1E9);
	numa_node_size(1, &freep);
	printf("freep in GB on 1: %f\n", (float) freep / 1E9);

	// startThreadedMemoryPenetration();

	// return;









	double *first = (double *) numa_alloc_onnode(ndim * ndim * sizeof(double), destnode);
	double *second = (double *) numa_alloc_onnode(ndim * ndim * sizeof(double), destnode);
	double *multiply = (double *) numa_alloc_onnode(ndim * ndim * sizeof(double), destnode);
	int i, c, d;

	numa_run_on_node(0);

	for (c = 0 ; c < ndim; c++)
	{
	  for (d = 0 ; d < ndim; d++)
	  {
	    first[IDX(c,d)] = rand() % 100; //int between 0 - 100
	    second[IDX(c,d)] = rand() % 100;
	    multiply[IDX(c,d)] = 0;
	  }
	}


  for (i = 1; i < argc; ++i) {
    if (strcmp("0", argv[i]) == 0) {
    	printf("parallelNaive\n");
			parallelNaive(first, second, multiply);
    }

    if (strcmp("1", argv[i]) == 0) {
			for (c = 0 ; c < ndim; c++) {
			  for (d = 0 ; d < ndim; d++) {
			    multiply[IDX(c,d)] = 0;
			  }
			}
    	printf("naiveMultiplication\n");
			naiveMultiplication(first, second, multiply);
		}
    if (strcmp("2", argv[i]) == 0) {
  		for (c = 0 ; c < ndim; c++) {
			  for (d = 0 ; d < ndim; d++) {
			    multiply[IDX(c,d)] = 0;
			  }
			}
    	printf("strassenMultiplication\n");
			strassenMultiplication(ndim, first, second, multiply);
		}
    if (strcmp("3", argv[i]) == 0) {
  		for (c = 0 ; c < ndim; c++) {
			  for (d = 0 ; d < ndim; d++) {
			  	multiply[IDX(c,d)] = 0;
			  }
			}
    	printf("parallelSum\n");
			parallelSum(first, second, multiply);
		}

    if (strcmp("5", argv[i]) == 0) {
  		for (c = 0 ; c < ndim; c++) {
			  for (d = 0 ; d < ndim; d++) {
			  	multiply[IDX(c,d)] = 0;
			  }
			}
    	printf("doubleBlockedMultiply\n");
			doubleBlockedMultiply(first, second, multiply);
		}
  }

	return 0;
}


char** buffers;

void* penetrateMemory(void *args) {

	threadArguments *a = (threadArguments*) args;

	printf("i was spawned. from: %d to: %d\n", a->from, a->to);
	size_t bufferSize = 10 * 1E9 * sizeof(char);



	memcpy(buffers[a->to], buffers[a->from], bufferSize);
	return NULL;
}

void startThreadedMemoryPenetration() {

  pthread_t thread[NUM_THREADS];
  pthread_attr_t attr;
  int rc;
  void *status;
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
  int threadcount = 10;
	size_t bufferSize = 10 * 1E9 * sizeof(char);
  buffers = malloc(2 * threadcount * sizeof(char*));
	threadArguments *threadArgs = malloc(threadcount * sizeof(threadArguments));
	printf("up to date?\n");
  for (int i = 0; i < threadcount; i++) {
  	buffers[2 * i] = (char *) numa_alloc_onnode(bufferSize, (int) (2 * i % NUM_NODES));
  	buffers[2 * i + 1] = (char *) numa_alloc_onnode(bufferSize, (int) ((2 * i + 1) % NUM_NODES));
  	threadArgs[i].from = 2 * i;
  	threadArgs[i].to = 2 * i + 1 < threadcount * 2 ? 2 * i + 1 : 0;
  }
	for (int i = 0; i < threadcount; ++i)
	{
	  rc = pthread_create(&thread[i],  &attr, penetrateMemory, (void*) &threadArgs[i]);
    // rc = pthread_join(thread[i], &status);
	}

  pthread_attr_destroy(&attr);
  for (int i = 0; i < NUM_THREADS; i++) {
    rc = pthread_join(thread[i], &status);
    if (rc) {
       printf("ERROR; return code from pthread_join() is %d\n", rc);
       exit(EXIT_FAILURE);
    }
  }


	printf("start...\n");

	// for (int i = 0; i < bufferSize / 1000; ++i)
	// {
	// 	memcpy(buffer2, buffer1, i * 1000);
	// 	memcpy(buffer3, buffer1, i * 1000);
	// 	memcpy(buffer4, buffer1, i * 1000);

	// 	memcpy(buffer1, buffer2, i * 1000);
	// 	memcpy(buffer3, buffer2, i * 1000);
	// 	memcpy(buffer4, buffer2, i * 1000);

	// 	memcpy(buffer1, buffer3, i * 1000);
	// 	memcpy(buffer2, buffer3, i * 1000);
	// 	memcpy(buffer4, buffer3, i * 1000);

	// 	memcpy(buffer1, buffer4, i * 1000);
	// 	memcpy(buffer2, buffer4, i * 1000);
	// 	memcpy(buffer3, buffer4, i * 1000);
	// }

	printf("end\n");
}