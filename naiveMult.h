/*naiveMult.h*/

#pragma once

void parallelNaive(double first[], double second[], double multiply[]);
void naiveMultiplication(double first[], double second[], double multiply[]);

int NUM_THREADS;
int NUM_NODES;
int ndim;
int halfMatrixCellCount;
size_t halfMatrixSize;