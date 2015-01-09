#include <stdio.h>
#include <time.h>
#include <time.h>
#include <sys/time.h>
#include <stdlib.h>
#include <string.h>
#include "naiveMult.h"
#include "strassenutil.h"
#include "globals.h"


#define IDX(Y, X) (ndim * Y + X) //rows first

const int NUM_THREADS = 4;
const int ndim = 512;

int main(int argc, char **argv)
{


    srand(1);
    double *first = malloc(ndim * ndim * sizeof(double));
    double *second = malloc(ndim * ndim * sizeof(double));
    double *multiply = malloc(ndim * ndim * sizeof(double));
    int i, c, d;
    printf("NUM_THREADS: %d\n", NUM_THREADS);
    printf("ndim: %d\n", ndim);
    setbuf(stdout, NULL);


    for (c = 0 ; c < ndim; c++)
    {
        for (d = 0 ; d < ndim; d++)
        {
            first[IDX(c, d)] = rand() % 10; //int between 0 - 1000
            second[IDX(c, d)] = rand() % 10;
            multiply[IDX(c, d)] = 0;

        }
    }

    // for (c = 0 ; c < ndim; c++)
    // {
    //     for (d = 0 ; d < ndim; d++)
    //     {
    //         printf("%f ", first[IDX(c, d)]);
    //     }
    //     printf("\n");
    // }
    // printf("\n");
    // for (c = 0 ; c < ndim; c++)
    // {
    //     for (d = 0 ; d < ndim; d++)
    //     {
    //         printf("%f ", second[IDX(c, d)]);
    //     }
    //     printf("\n");
    // }

    for (i = 1; i < argc; ++i)
    {
        if (strcmp("0", argv[i]) == 0)
            parallelNaive(first, second, multiply);
        if (strcmp("1", argv[i]) == 0)
        {
            for (c = 0 ; c < ndim; c++)
            {
                for (d = 0 ; d < ndim; d++)
                {
                    multiply[IDX(c, d)] = 0;
                }
            }
            naiveMultiplication(first, second, multiply);
        }
        if (strcmp("2", argv[i]) == 0)
        {
            for (c = 0 ; c < ndim; c++)
            {
                for (d = 0 ; d < ndim; d++)
                {
                    multiply[IDX(c, d)] = 0;
                }
            }
            strassenParallel(ndim, first, second, multiply);
            isValid(first, second, multiply);
        }
    }

    return 0;
}