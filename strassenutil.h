#include "strassen.h"

/*whole strassen mulitplication, created from flat arrays*/
void strassenMultiplication(int n, float first[], float second[], float multiply[]);

void strassenParallel(int n, float first[], float second[], float multiply[]);
void strassenParallelNUMA(int n, float first[], float second[], float multiply[]);

matrix strassen_newmatrix(int);		/* allocate storage */
matrix strassen_newmatrix_block(int n);
matrix strassen_newmatrix_block_NUMA(int n, int node);
int sizeofMatrix(int n);

void strassen_randomfill(int, matrix);	/* fill with random values in the
										* the range [0,1) */

void strassen_set(int n, matrix a, float flatMatrix[], int startRow, int startColumn);
void strassen_get(int n, matrix a, float flatMatrix[], int startRow, int startColumn);
void strassen_naivemult(int, matrix, matrix, matrix);	/* classical algorithm */
void strassen_discrepancy(int, matrix, matrix);	/* print places where
												* matrices differ */
int diff(float, float);	/* do two floats differ? */
void check(int, char *);	/* check for error conditions */

void printMatrix(int n, matrix a);
