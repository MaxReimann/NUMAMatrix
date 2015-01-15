/*naiveMult.h*/

#pragma once

void parallelNaive(long first[], long second[], long multiply[]);
void naiveMultiplication(long first[], long second[], long multiply[]);
void primitiveMultiply_withBlocking(long *A, long *B, long *C,
  int startColumn, int lastColumn);
void primitiveMultiply_withoutBlocking(long *first, long *second, long *multiply,
  int startColumn, int lastColumn);

void parallelSum(long first[], long second[], long multiply[]);
void doubleBlockedMultiply(long A[], long B[], long C[]);

int NUM_THREADS;
int NUM_NODES;
int ndim;
int halfMatrixCellCount;
size_t halfMatrixSize;