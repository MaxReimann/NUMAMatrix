#include "strassen.h"

/*whole strassen mulitplication, created from flat arrays*/
void strassenMultiplication(int n, double first[], double second[], double multiply[]);

void strassenParallel(int n, double first[], double second[], double multiply[]);

matrix strassen_newmatrix(int);		/* allocate storage */
matrix strassen_newmatrix_block(int n);
matrix strassen_newmatrix_block_NUMA(int n, int node);
int sizeofMatrix(int n);

void strassen_randomfill(int, matrix);	/* fill with random values in the
										* the range [0,1) */

void strassen_set(int n, matrix a, double flatMatrix[], int startRow, int startColumn);
void strassen_get(int n, matrix a, double flatMatrix[], int startRow, int startColumn);
void strassen_naivemult(int, matrix, matrix, matrix);	/* classical algorithm */
void strassen_discrepancy(int, matrix, matrix);	/* print places where
												* matrices differ */
int diff(double, double);	/* do two doubles differ? */
void check(int, char *);	/* check for error conditions */

void printMatrix(int n, matrix a);
