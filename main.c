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


int indexOfParameter(int argc, char **argv, char* parameter);

int NUM_THREADS = 49;
int ndim = 2000;

int main(int argc, char **argv)
{

    int indexThreadNum = indexOfParameter(argc, argv, "-t");
    if (indexThreadNum!=-1)
        NUM_THREADS = atoi(argv[indexThreadNum+1]);

    int indexDimension = indexOfParameter(argc, argv, "-n");
    if (indexDimension!=-1)
        ndim = atoi(argv[indexDimension+1]);


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
    
    for (i = 1; i < argc; ++i)
    {
        if (strcmp("0", argv[i]) == 0)
            parallelNaive(first, second, multiply);
        else if (strcmp("1", argv[i]) == 0)
        {
            for (c = 0 ; c < ndim; c++)
            {
                for (d = 0 ; d < ndim; d++)
                    multiply[IDX(c, d)] = 0;
            }

            int blockSize = 100;
            if (i+1 < argc && strcmp(argv[i+1],"-b")==0)
            {
                blockSize = atoi(argv[i+2]);
                i+=2;
                printf("using blocksize: %d\n",blockSize);
            }
            blockedMultiply(first, second, multiply, blockSize);
            //isValid(first, second, multiply);

            // for (c = 0 ; c < ndim; c++)
            // {
            //     for (d = 0 ; d < ndim; d++)
            //         multiply[IDX(c, d)] = 0;
            // }
            // naiveMultiplication(first, second, multiply);
        }
        else if (strcmp("2", argv[i]) == 0)
        {
            for (c = 0 ; c < ndim; c++)
            {
                for (d = 0 ; d < ndim; d++)
                    multiply[IDX(c, d)] = 0;
            }
            //strassenMultiplication(ndim, first, second, multiply);
            strassenMassiveParallel(ndim, first, second, multiply);
            //isValid(first, second, multiply);
        }

        else if (strcmp("3", argv[i]) == 0)
        {
            for (c = 0 ; c < ndim; c++)
            {
                for (d = 0 ; d < ndim; d++)
                    multiply[IDX(c, d)] = 0;
            }
            //strassenMultiplication(ndim, first, second, multiply);
            strassenMassiveParallelNUMA(ndim, first, second, multiply);
            //isValid(first, second, multiply);
        }
    }

    return 0;
}

int indexOfParameter(int argc, char **argv, char* parameter)
{
    for (int i = 1; i < argc; ++i)
    {
        if (strcmp(parameter, argv[i]) == 0) 
            return i;
    }
    return -1;
}