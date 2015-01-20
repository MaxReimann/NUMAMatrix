/*naiveMult.h*/
#pragma once

#include "globals.h"

void naiveMultiplication(float first[], float second[], float multiply[]);
bool isValid(float first[], float second[], float multiplied[]);

void blockedMultiply(float A[], float B[], float C[]);
void naiveSSEMultiplication(float A[], float B[], float C[]);
void blockedSSEMultiply(float A[], float B[], float C[], int blockSize);


void parallelNaiveBlocked(float first[], float second[], float multiply[]);
void parallelNaiveSSE(float first[], float second[], float multiply[]);