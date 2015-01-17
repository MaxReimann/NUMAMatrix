#include <stdio.h>
#include <time.h>
#include <time.h>
#include <sys/time.h>
#include <stdlib.h>
#include <string.h>
#include "naiveMult.h"
#include "strassenutil.h"
#include "globals.h"
#include <assert.h>     /* assert */
#include "BLASmult.h"
#include <mm_malloc.h>

#define IDX(Y, X) (ndim * Y + X) //rows first


int indexOfParameter(int argc, char **argv, char* parameter);
int findBreakSize();

int NUM_THREADS = 49;
int ndim = 4096;
int BREAK = 32;


int main(int argc, char **argv)
{
    msleep(100);

    int indexThreadNum = indexOfParameter(argc, argv, "-t");
    if (indexThreadNum!=-1)
        NUM_THREADS = atoi(argv[indexThreadNum+1]);

    int indexDimension = indexOfParameter(argc, argv, "-n");
    if (indexDimension!=-1)
        ndim = atoi(argv[indexDimension+1]);

    BREAK = findBreakSize();
    assert(BREAK!=-1);

    srand(1);
    float *first = _mm_malloc(ndim * ndim * sizeof(float),16);
    float *second = _mm_malloc(ndim * ndim * sizeof(float),16);
    float *multiply = _mm_malloc(ndim * ndim * sizeof(float),16);
    int i, c, d;
    //printf("NUM_THREADS: %d\n", NUM_THREADS);
    //printf("ndim: %d\n", ndim);
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
        if (strcmp("#", argv[i]) == 0) //end of mode declarations
        {
            break;
        }


        if (strcmp("0", argv[i]) == 0){
                    printf("running blocked\n");
            blockedMultiply(first, second, multiply);
        }
        else if (strcmp("1", argv[i]) == 0)
        {
            for (c = 0 ; c < ndim; c++)
            {
                for (d = 0 ; d < ndim; d++)
                    multiply[IDX(c, d)] = 0;
            }

            strassenMultiplication(ndim, first, second, multiply);
            //isValid(first, second, multiply);

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

        else if (strcmp("4", argv[i]) == 0)
        {
            for (c = 0 ; c < ndim; c++)
            {
                for (d = 0 ; d < ndim; d++)
                    multiply[IDX(c, d)] = 0;
            }
            //strassenMultiplication(ndim, first, second, multiply);
            square_sgemm(first, second, multiply);
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


int findBreakSize()
{
    if (ndim%BREAK!=0)
    {
        int i_min = (BREAK-10 >= 4)? BREAK-10 : 4;
        int i_max = (BREAK+10 < ndim)? BREAK+10 : ndim-1;
        for (int i=i_min; i <= i_max; i++)
        {
            if (ndim%BREAK == 0 && BREAK%4==0) //must be divdable by 4 for vecotrization
                return i;
        }
        return -1;
    }
    else
        return BREAK;
}