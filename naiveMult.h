/*naiveMult.h*/
#pragma once

#include "globals.h"

void parallelNaive(float first[], float second[], float multiply[]);
void naiveMultiplication(float first[], float second[], float multiply[]);
bool isValid(float first[], float second[], float multiplied[]);

void blockedMultiply(float A[], float B[], float C[]);
