#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#include "naiveMult.h"

// #define ndim  1024

int main()
{
	NUM_THREADS = 2;
	ndim = 1024;

	srand(time(NULL));
	double *first=malloc(ndim*ndim*sizeof(double));
	double *second=malloc(ndim*ndim*sizeof(double));
	double *multiply=malloc(ndim*ndim*sizeof(double));

	parallelNaive(first,second, multiply);
	naiveMultiplication(first,second, multiply);
	strassenMultiplication(ndim, first,second, multiply);

	return 0;
}