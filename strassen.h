#pragma once
/*
* strassen.h
*
* Copurtesy [Buhler 1993].
* Header file for strassen matrix multiplication functions.
*/


/*
* A matrix is defined to be a pointer to a ``union _matrix'', which
* is (if the size is <= BREAK) a matrix of numbers, or else is an
* array of four submatrices of half size.
*/

typedef union _matrix {
	float *d;
	union _matrix **p;
} *matrix;



void strassen_multiply(int, matrix, matrix, matrix, matrix);
void add(int, matrix, matrix, matrix);
void sub(int, matrix, matrix, matrix);

/*
* Notational shorthand to access submatrices for matrices named
* a,b,c,d
*/

#define a11 a->p[0]
#define a12 a->p[1]
#define a21 a->p[2]
#define a22 a->p[3]
#define b11 b->p[0]
#define b12 b->p[1]
#define b21 b->p[2]
#define b22 b->p[3]
#define c11 c->p[0]
#define c12 c->p[1]
#define c21 c->p[2]
#define c22 c->p[3]
#define d11 d->p[0]
#define d12 d->p[1]
#define d21 d->p[2]
#define d22 d->p[3]
#define p11 p->p[0]
#define p12 p->p[1]
#define p21 p->p[2]
#define p22 p->p[3]
#define p11 p->p[0]
#define p12 p->p[1]
#define p_temp p->p[2]
#define p_res p->p[3]