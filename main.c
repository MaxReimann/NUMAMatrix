#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include "naiveMult.h"

// #define ndim  1024

int main(int argc, char **argv)
{
	NUM_THREADS = 2;
	ndim = 1024;

	srand(time(NULL));
	double *first=malloc(ndim*ndim*sizeof(double));
	double *second=malloc(ndim*ndim*sizeof(double));
	double *multiply=malloc(ndim*ndim*sizeof(double));
	int i;
	printf("NUM_THREADS: %d\n", NUM_THREADS);
	printf("ndim: %d\n", ndim);

  for (i = 1; i < argc; ++i) {
    if (strcmp("0", argv[1]) == 0)
			parallelNaive(first,second, multiply);
    if (strcmp("1", argv[1]) == 0)
			naiveMultiplication(first,second, multiply);
    if (strcmp("2", argv[1]) == 0)
			strassenMultiplication(ndim, first,second, multiply);
  }

	return 0;
}