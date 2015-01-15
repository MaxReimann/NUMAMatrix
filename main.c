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


#include "membench.h"

// #define ndim  1024
#define IDX(Y, X) (ndim * Y + X) //rows first

int main(int argc, char **argv)
{
	NUM_THREADS = 128;
	ndim = 4096;
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