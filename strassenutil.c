
/*
* strassenutil.h
*
* Courtesy [Buhler 1993].
* A test file for the Strassen matrix multiplication functions.
* Utility functions for matrix creation and checking
*
*/


#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <stdlib.h>
#include <sched.h>
#include <numa.h>

#include "strassenutil.h"
#include "globals.h"
#define BILLION 1E9


/* Fill the matrix a with random values between 0 and 1 */
void strassen_randomfill(int n, matrix a)
{
    if (n <= BREAK)
    {
        int i, j;
        double **p = a->d, T = -(double)(1 << 31);

        for (i = 0; i < n; i++)
            for (j = 0; j < n; j++)
                p[i][j] = rand() / T;
    }
    else
    {
        n /= 2;
        strassen_randomfill(n, a11);
        strassen_randomfill(n, a12);
        strassen_randomfill(n, a21);
        strassen_randomfill(n, a22);
    }
}


/* Fill the matrix from a flat matrix*/
void strassen_set(int n, matrix a, double flatMatrix[], int startRow, int startColumn)
{
#define INDEX(Y, X) (ndim * Y + X) //rows first
    if (n <= BREAK)
    {
        int i, j;
        double **p = a->d;

        for (i = 0; i < n; i++)
        {
            for (j = 0; j < n; j++)
                p[i][j] = flatMatrix[INDEX((startRow + i), (startColumn + j))];

        }




    }
    else
    {
        n /= 2;
        strassen_set(n, a11, flatMatrix, startRow, startColumn);
        strassen_set(n, a12, flatMatrix, startRow, startColumn + n);
        strassen_set(n, a21, flatMatrix, startRow + n, startColumn);
        strassen_set(n, a22, flatMatrix, startRow + n, startColumn + n);
    }
}


/* Write matrix into flat array*/
void strassen_get(int n, matrix a, double outputMat[], int startRow, int startColumn)
{
#define INDEX(Y, X) (ndim * Y + X) //rows first
    if (n <= BREAK)
    {
        int i, j;
        double **p = a->d;

        for (i = 0; i < n; i++)
        {
            for (j = 0; j < n; j++)
            {
                outputMat[INDEX((startRow + i), (startColumn + j))] = p[i][j];
                // printf("%f ", p[i][j] );
            }
            // printf("\n");
        }


    }
    else
    {
        n /= 2;
        strassen_get(n, a11, outputMat, startRow, startColumn);
        strassen_get(n, a12, outputMat, startRow, startColumn + n);
        strassen_get(n, a21, outputMat, startRow + n, startColumn);
        strassen_get(n, a22, outputMat, startRow + n, startColumn + n);
    }
}

void strassen_naivemult(int n, matrix a, matrix b, matrix c)    /* c = a*b */
{
    matrix scratch;

    if (n <= BREAK)
    {
        double sum, **p = a->d, **q = b->d, **r = c->d;
        int i, j, k;

        for (i = 0; i < n; i++)
        {
            for (j = 0; j < n; j++)
            {
                for (sum = 0., k = 0; k < n; k++)
                    sum += p[i][k] * q[k][j];
                r[i][j] = sum;
            }
        }
    }
    else
    {
        n /= 2;
        scratch = strassen_newmatrix(n);
        strassen_naivemult(n, a11, b11, scratch);
        strassen_naivemult(n, a12, b21, c11);
        add(n, scratch, c11, c11);
        strassen_naivemult(n, a11, b12, scratch);
        strassen_naivemult(n, a12, b22, c12);
        add(n, scratch, c12, c12);
        strassen_naivemult(n, a21, b11, scratch);
        strassen_naivemult(n, a22, b21, c21);
        add(n, scratch, c21, c21);
        strassen_naivemult(n, a21, b12, scratch);
        strassen_naivemult(n, a22, b22, c22);
        add(n, scratch, c22, c22);
        /* we should free scratch at this point but we are lazy */
    }
}

int sizeofMatrix(int n)
{
    matrix a;
    double alloc = sizeof(*a);

    if (n <= BREAK){

        alloc += n * sizeof(double*);
        for (int i = 0; i < n; i++)
            alloc += n * sizeof(double);
    }
    else 
    {
            n/=2;
            alloc += 4 * sizeof(matrix);
    
            alloc += sizeofMatrix(n);
            alloc += sizeofMatrix(n);
            alloc += sizeofMatrix(n);
            alloc += sizeofMatrix(n);
    }
        
    return alloc;
}

void *my_malloc(void  *memory, int *memPointer,int size)
{
    // forward beginning of memory by mempointer
    // note this kind of void pointer arithmetics is only allowed 
    // by gcc
    void *mem = memory + *memPointer;
    *memPointer += size;
    return mem;
}

matrix _strassen_newmatrix_block(int n, void  *memory, int *memPointer)
{
    matrix a = (matrix) my_malloc(memory, memPointer, sizeof(*a));

    if (n<= BREAK)
    {
        a->d = (double **) my_malloc(memory, memPointer, n * sizeof(double*));

        for (int i = 0; i < n; i++)
        {
            a->d[i] =  my_malloc(memory, memPointer, n * sizeof(double));
        }
    }
    else
    {
        n /= 2;
        a->p =  (matrix *) my_malloc(memory, memPointer, 4 * sizeof(double));

        a11 = _strassen_newmatrix_block(n, memory, memPointer);
        a12 = _strassen_newmatrix_block(n, memory, memPointer);
        a21 = _strassen_newmatrix_block(n, memory, memPointer);
        a22 = _strassen_newmatrix_block(n, memory, memPointer);
    }
    return a;

}

matrix strassen_newmatrix_block(int n)
{
    int size = sizeofMatrix(n);
    void *memory=malloc(size);
    //printf("memsize %d\n", size);

    int memPointer = 0;
    return _strassen_newmatrix_block(n, memory,&memPointer);
}

matrix strassen_newmatrix_block_NUMA(int n, int node)
{
    int size = sizeofMatrix(n);
    void *memory=numa_alloc_onnode(size, node);
    //printf("memsize %d\n", size);

    int memPointer = 0;
    return _strassen_newmatrix_block(n, memory,&memPointer);
}



/* return new square n by n matrix */
matrix strassen_newmatrix(int n)
{
    matrix a;

    a = (matrix)malloc(sizeof(*a));
    check(a != NULL, "newmatrix: out of space for matrix");
    if (n <= BREAK)
    {
        int i;

        a->d = (double **)calloc(n, sizeof(double *));
        check(a->d != NULL,
              "newmatrix: out of space for row pointers");
        for (i = 0; i < n; i++)
        {
            a->d[i] = (double *)calloc(n, sizeof(double));
            check(a != NULL, "newmatrix: out of space for rows");
        }
    }
    else
    {
        n /= 2;
        a->p = (matrix *)calloc(4, sizeof(matrix));
        check(a->p != NULL,
              "newmatrix: out of space for submatrices");
        a11 = strassen_newmatrix(n);
        a12 = strassen_newmatrix(n);
        a21 = strassen_newmatrix(n);
        a22 = strassen_newmatrix(n);
    }
    return a;
} 
// n-BREAK

/*
* Print the first MAXERRS locations where the two matrices differ
*/

#define MAXERRS 100

void strassen_discrepancy(int n, matrix a, matrix b)
{
    static int errors = 0;

    if (errors >= MAXERRS)
        return;
    if (n <= BREAK)
    {
        int i, j;
        double **p = a->d, **q = b->d;

        for (i = 0; i < n; i++)
        {
            for (j = 0; j < n; j++)
            {
                if (diff(p[i][j], q[i][j]))
                {
                    if (errors++ >= MAXERRS)
                        return;
                    else
                    {
                        printf("strassen[%d][%d] = %.12f, ",
                               i, j, p[i][j]);
                        printf("classical = %.12f\n", q[i][j]);
                    }
                }
            }
        }
    }
    else
    {
        n /= 2;
        strassen_discrepancy(n, a11, b11);
        strassen_discrepancy(n, a12, b12);
        strassen_discrepancy(n, a21, b21);
        strassen_discrepancy(n, a22, b22);
    }
}

/*
* Are two doubles significantly different?  Since the original
* numbers are between 0 and 1, we arbitrarily decide they are
* different if their difference is at most 1e-12.  This is somewhat
* fragile, and could be replaced by a consideration of their
* relative difference.
*/

int diff(double x, double y)
{
    x -= y;
    if (x < 0)
        x = -x;
    return x > 1e-12;
}

/*
* If the expression e is false print the error message s and quit.
*/

void check(int e, char *s)
{
    if (!e)
    {
        fprintf(stderr, "Fatal error -> %s\n", s);
        exit(1);
    }
}



void printMatrix(int n, matrix a)
{
#define INDEX(Y, X) (ndim * Y + X) //rows first

    double *multiply = malloc(ndim * ndim * sizeof(double));
    strassen_get(n, a, multiply, 0, 0);


    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
            printf("%f ", multiply[INDEX(i,j)] );
        printf("\n");
    }

    printf("\n");


}

void strassenMultiplication(int n, double first[], double second[], double multiply[])
{

    printf("Running strassenMultiplication\n");
    matrix a, b, c, d;
    a = strassen_newmatrix_block(n);
    b = strassen_newmatrix_block(n);
    c = strassen_newmatrix_block(n);
    d = strassen_newmatrix_block(n);
    strassen_set(n, a, first, 0 , 0);
    strassen_set(n, b, second, 0, 0);


    struct timespec start, end;
    clock_gettime(CLOCK_THREAD_CPUTIME_ID, &start);

    strassen_multiply(n, a, b, c, d);  /* strassen algorithm */

    clock_gettime(CLOCK_THREAD_CPUTIME_ID, &end);
    float seconds = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / BILLION;

    strassen_get(n, c, multiply, 0, 0);
    printf("strassenMultiplication took:%f seconds\n\n", seconds);
}
