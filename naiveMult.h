/*naiveMult.h*/

#pragma once

void parallelNaive(double first[], double second[], double multiply[]);
void naiveMultiplication(double first[], double second[], double multiply[]);
void primitiveMultiply_withBlocking(double *A, double *B, double *C,
  int startColumn, int lastColumn);
void primitiveMultiply_withoutBlocking(double *first, double *second, double *multiply,
  int startColumn, int lastColumn);

void parallelSum(double first[], double second[], double multiply[]);
void doubleBlockedMultiply(double A[], double B[], double C[]);

int NUM_THREADS;
int NUM_NODES;
int ndim;
int halfMatrixCellCount;
size_t halfMatrixSize;

typedef enum { false, true } bool;

bool useBlocking;