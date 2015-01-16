#include <stdio.h>
#include <time.h>

//constants needed everywhere

//ndim must be dividable by NUM_THREADS
extern int NUM_THREADS;
extern int ndim;


/*
* Square matrices of size <= BREAK are handled with the ``classical
* algorithms.  The shape of almost all functions is therefore
* something like
*
*	if ( n <= BREAK )
*	    classical algorithms
*	else
*	    n/= 2
*	    recursive call for 4 half-size submatrices
*/

//NDIM must be divdable by BREAK!

extern int BREAK;
