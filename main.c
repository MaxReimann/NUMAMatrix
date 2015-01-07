#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include "naiveMult.h"

// #define ndim  1024
#define IDX(Y, X) (ndim * Y + X) //rows first

int main(int argc, char **argv)
{
	NUM_THREADS = 4;
	ndim = 1024;

	srand(time(NULL));
	double *first=malloc(ndim*ndim*sizeof(double));
	double *second=malloc(ndim*ndim*sizeof(double));
	double *multiply=malloc(ndim*ndim*sizeof(double));
	int i, c, d;
	printf("NUM_THREADS: %d\n", NUM_THREADS);
	printf("ndim: %d\n", ndim);


	for (c = 0 ; c < ndim; c++)
	{
	  for (d = 0 ; d < ndim; d++)
	  {
	    first[IDX(c,d)] = rand() % 100; //int between 0 - 1000
	    second[IDX(c,d)] = rand() % 100;
	    multiply[IDX(c,d)] = 0;
	  }
	}


  for (i = 1; i < argc; ++i) {
    if (strcmp("0", argv[i]) == 0)
			parallelNaive(first,second, multiply);
    if (strcmp("1", argv[i]) == 0) {
			for (c = 0 ; c < ndim; c++) {
			  for (d = 0 ; d < ndim; d++) {
			    multiply[IDX(c,d)] = 0;
			  }
			}
			naiveMultiplication(first,second, multiply);
		}
    if (strcmp("2", argv[i]) == 0) {
  		for (c = 0 ; c < ndim; c++) {
			  for (d = 0 ; d < ndim; d++) {
			    multiply[IDX(c,d)] = 0;
			  }
			}
			strassenMultiplication(ndim, first,second, multiply);
		}
  }

	return 0;
}