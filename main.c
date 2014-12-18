#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#include "naiveMult.h"

#define ndim  1024

int main()
{
	srand(time(NULL));
	double *first=malloc(ndim*ndim*sizeof(double));
	double *second=malloc(ndim*ndim*sizeof(double));
	double *multiply=malloc(ndim*ndim*sizeof(double));

	parallelNaive(ndim,first,second, multiply);
	strassenMultiplication(ndim,first,second, multiply);

	return 0;
}