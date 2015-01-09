#include <stdio.h>
#include "strassenutil.h"
#include "strassen.h"



void strassen_P1 (int n, matrix A11, matrix A22, 
				matrix B11, matrix B22,
				matrix p)
// P1 = (A11 + A22)*(B11+B22)
{

	add(n, A11, A22, p11);

	add(n, B11, B22, p12);
	strassen_multiply(n, p11, p12, p_res, p_temp); 
}

void strassen_P2 (int n, matrix A21, matrix A22, 
				matrix B11,
				matrix p)
//  P2 = (A21 + A22)B11
{

	add(n, A21, A22, p11);
	strassen_multiply(n, p11, B11, p_res, p_temp); 
}

void strassen_P3 (int n, matrix A11, 
				matrix B12, matrix B22,
				matrix p)
//  P3 =  A11*(B12-B22)
{
	sub(n, B12, B22, p11);
	strassen_multiply(n, p11, A11, p_res, p_temp); 
}

void strassen_P4 (int n, matrix A22, 
				matrix B21, matrix B11,
				matrix p)
//  P4 =  A22*(B21-B11)
{
	sub(n, B21, B11, p11);
	strassen_multiply(n, p11, A22, p_res, p_temp); 
}



void strassen_P5 (int n, matrix A11, matrix A12, 
				matrix B22,
				matrix p)
//  P5 = (A11 + A12)B22 
{

	add(n, A11, A12, p11);
	strassen_multiply(n, p11, B22, p_res, p_temp); //p1
}


void strassen_P6 (int n, matrix A21, matrix A11, 
				matrix B11, matrix B12,
				matrix p)
//   P6 = (A21-A11)*(B11+B12) 
{

	add(n, B11, B12, p11);
	sub(n, A21, A11, p12);
	strassen_multiply(n, p11, p12, p_res, p_temp); 
}

void strassen_P7 (int n, matrix A12, matrix A22, 
				matrix B21, matrix B22,
				matrix p)
//   P7 = (A12-A22)*(B21+B22)
{

	sub(n, A12, A22, p11);
	add(n, B21, B22, p12);
	strassen_multiply(n, p11, p12, p_res, p_temp); 
}



void strassenParallel(int n, double first[], double second[], double multiply[])
{
	printf("Running parallel strassenMultiplication\n");
	matrix a, b, c, P1, P2, P3, P4, P5, P6, P7;
	a = strassen_newmatrix(n);
	b = strassen_newmatrix(n);
	c = strassen_newmatrix(n);
	P1 = strassen_newmatrix(n);
	P2 = strassen_newmatrix(n);
	P3 = strassen_newmatrix(n);
	P4 = strassen_newmatrix(n);
	P5 = strassen_newmatrix(n);
	P6 = strassen_newmatrix(n);
	P7 = strassen_newmatrix(n);


	strassen_set(n, a, first, 0 ,0);
	strassen_set(n, b, second, 0, 0);

	n /= 2;

	strassen_P1(n, a11, a22, b11, b22, P1);
	strassen_P2(n, a21, a22, b11, P2);
	strassen_P3(n, a11, b12, b22, P3);
	strassen_P4(n, a22, b21, b11, P4);
	strassen_P5(n, a11, a12, b22, P5);
	strassen_P6(n, a21, a11, b11, b12, P6);
	strassen_P7(n, a12, a22, b21, b22, P7);

	//  C11 = P1 + P4 - P5 + P7
	//  C22 = P1 - P2 + P3 + P6

	add(n, P1->p[3], P4->p[3], c11);
	sub(n, c11, P5->p[3], c11);
	add(n, c11, P7->p[3], c11);

	sub(n, P1->p[3], P2->p[3], c22);
	add(n, c22, P3->p[3], c22);
	add(n, c22, P6->p[3], c22);



}

