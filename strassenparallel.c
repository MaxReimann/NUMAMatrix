#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "globals.h"
#include "strassenutil.h"
#include "strassen.h"

#define BILLION 1E9

// Parts of strassen decomposition

void strassen_P1 (int n, matrix A11, matrix A22, //yes
                  matrix B11, matrix B22,
                  matrix p)
// P1 = (A11 + A22)*(B11+B22)
{

    add(n, A11, A22, p11);

    add(n, B11, B22, p12);
    strassen_multiply(n, p11, p12, p_res, p_temp);

}

void strassen_P2 (int n, matrix A21, matrix A22,
                  matrix B11,
                  matrix p)
//  P2 = (A21 + A22)B11
{

    add(n, A21, A22, p11);
    strassen_multiply(n, p11, B11, p_res, p_temp);
}

void strassen_P3 (int n, matrix A11,
                  matrix B12, matrix B22,
                  matrix p)
//  P3 =  A11*(B12-B22)
{

    sub(n, B12, B22, p11);

    strassen_multiply(n, A11, p11, p_res, p_temp);
}

void strassen_P4 (int n, matrix A22,
                  matrix B21, matrix B11,
                  matrix p)
//  P4 =  A22*(B21-B11)
{
    sub(n, B21, B11, p11);
    strassen_multiply(n, A22, p11, p_res, p_temp);
}



void strassen_P5 (int n, matrix A11, matrix A12,
                  matrix B22,
                  matrix p)
//  P5 = (A11 + A12)B22
{

    add(n, A11, A12, p11);
    strassen_multiply(n, p11, B22, p_res, p_temp); //p1
}


void strassen_P6 (int n, matrix A21, matrix A11,
                  matrix B11, matrix B12,
                  matrix p)
//   P6 = (A21-A11)*(B11+B12)
{

    sub(n, A21, A11, p11);
    add(n, B11, B12, p12);
    strassen_multiply(n, p11, p12, p_res, p_temp);
}

void strassen_P7 (int n, matrix A12, matrix A22,
                  matrix B21, matrix B22,
                  matrix p)
//   P7 = (A12-A22)*(B21+B22)
{

    sub(n, A12, A22, p11);
    add(n, B21, B22, p12);
    strassen_multiply(n, p11, p12, p_res, p_temp);
}


///////////////////////////////////
////// parallelization ////////////
///////////////////////////////////


typedef struct
{
    int n;
    matrix m1;
    matrix m2;
    matrix m3;
    matrix m4;
    matrix output;
    void (*fPtr1)(int, matrix, matrix, matrix, matrix, matrix);
    void (*fPtr2)(int, matrix, matrix, matrix, matrix);
    int index;

} threadArguments;

bool runningThreads[7] = {false, false, false, false, false, false, false};

void *parallelDispatcher(void *args)
{
    threadArguments *a = (threadArguments *) args;
    if (a->m4 != NULL)
        a->fPtr1(a->n, a->m1, a->m2, a->m3, a->m4, a->output);
    else
        a->fPtr2(a->n, a->m1, a->m2, a->m3,  a->output);

    runningThreads[a->index] = false;
    pthread_exit((void *) args);
}

int numRunning()
{
    int running = 0;
    for (int i = 0; i < 7; i++)
        if (runningThreads[i] == true)
            running++;
    return running;
}

int firstRunning()
{
    for (int i = 0; i < 7; i++)
        if (runningThreads[i] == true)
            return i;
    return -1;
}

void parallelExecuteParts(threadArguments parts[])
{
    pthread_attr_t attr;
    void *status;
    pthread_t thread[7];
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);


    for (int i = 0; i < 7; i++)
    {

        if (numRunning() >= NUM_THREADS)
        {
            //wait until one is free
            int rc = pthread_join(thread[firstRunning()], &status);
            if (rc)
            {
                fprintf(stderr, "%s: %s\n", "ERROR; return code from pthread_join()(1) ", strerror(rc));
                exit(EXIT_FAILURE);
            }
        }

        parts[i].index = i;
        int rc = pthread_create( &thread[i],  &attr, parallelDispatcher, (void *) &parts[i]);
        if (rc)
        {
            fprintf(stderr, "Error - pthread_create() return code: %d\n", rc);
            exit(EXIT_FAILURE);
        }
        runningThreads[i] = true;
    }


    /* Free attribute and wait for the other threads */
    pthread_attr_destroy(&attr);
    for (int i = 0; i < 7; i++)
    {
        if (runningThreads[i]==true)
        {
            int rc = pthread_join(thread[i], &status);
            if (rc)
            {
                fprintf(stderr, "%s: %s\n", "ERROR; return code from pthread_join()(2) ", strerror(rc));
                exit(EXIT_FAILURE);
            }
        }
    }


}



void strassenParallel(int n, float first[], float second[], float multiply[])
{
    printf("Running parallel strassenMultiplication\n");
    matrix a, b, c, P1, P2, P3, P4, P5, P6, P7;
    threadArguments parts[7];

    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    a = strassen_newmatrix(n);
    b = strassen_newmatrix(n);
    c = strassen_newmatrix(n);
    P1 = strassen_newmatrix(n);
    P2 = strassen_newmatrix(n);
    P3 = strassen_newmatrix(n);
    P4 = strassen_newmatrix(n);
    P5 = strassen_newmatrix(n);
    P6 = strassen_newmatrix(n);
    P7 = strassen_newmatrix(n);


    strassen_set(n, a, first, 0 , 0);
    strassen_set(n, b, second, 0, 0);

    n /= 2;

    parts[0] = (threadArguments)
    {
        n, a11, a22, b11, b22, P1, .fPtr1 = strassen_P1
    };
    parts[1] = (threadArguments)
    {
        n, a21, a22, b11, NULL, P2, .fPtr2 = strassen_P2
    };
    parts[2] = (threadArguments)
    {
        n, a11, b12, b22, NULL, P3, .fPtr2 = strassen_P3
    };
    parts[3] = (threadArguments)
    {
        n, a22, b21, b11, NULL, P4, .fPtr2 = strassen_P4
    };
    parts[4] = (threadArguments)
    {
        n, a11, a12, b22, NULL, P5, .fPtr2 = strassen_P5
    };
    parts[5] = (threadArguments)
    {
        n, a21, a11, b11, b12, P6, .fPtr1 = strassen_P6
    };
    parts[6] = (threadArguments)
    {
        n, a12, a22, b21, b22, P7, .fPtr1 = strassen_P7
    };

    //blocks until all parts are executed
    parallelExecuteParts(parts);

    //  add up parts to result
    //  C11 = P1 + P4 - P5 + P7
    //  C12 = P3 + P5
    //  C21 = P2 + P4
    //  C22 = P1 - P2 + P3 + P6

#define R(P) P->p[3]

    add(n, R(P1), R(P4), c11);
    sub(n, c11, R(P5), c11);
    add(n, c11, R(P7), c11);

    sub(n, R(P1), R(P2), c22);
    add(n, c22, R(P3), c22);
    add(n, c22, R(P6), c22);

    add(n, R(P3), R(P5), c12);

    add(n, R(P2), R(P4), c21);

    strassen_get(n * 2, c, multiply, 0, 0);

    clock_gettime(CLOCK_MONOTONIC, &end);
    float seconds = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / BILLION;

    printf("parallel strassen took: %f\n\n", seconds);
}


