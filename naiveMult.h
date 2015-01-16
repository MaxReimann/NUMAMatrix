/*naiveMult.h*/
#pragma once

#include "globals.h"

typedef enum { false, true } bool;

void parallelNaive(double first[], double second[], double multiply[]);
void naiveMultiplication(double first[], double second[], double multiply[]);
bool isValid(double first[], double second[], double multiplied[]);

void blockedMultiply(double A[], double B[], double C[]);
