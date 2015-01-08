#include "strassenutil.h"
#include "strassen.h"


void strassen_P1 (int n, matrix a11, matrix a22, 
				matrix b11, matrix b22,
				matrix p)
// P1 = (A11 + A22)*(B11+B22)
{

	add(n, a11, a22, p11);
	add(n, b11, b22, p12);
	strassen_multiply(n, p11, p12, p_res, p_temp); 
}

void strassen_P2 (int n, matrix a21, matrix a22, 
				matrix b11,
				matrix p)
//  P2 = (A21 + A22)B11
{

	add(n, a21, a22, p11);
	strassen_multiply(n, p11, b11, p_res, p_temp); 
}

void strassen_P3 (int n, matrix a11, 
				matrix b12, matrix b22
				matrix p)
//  P3 =  A11*(B12-B22)
{
	sub(n, b12, b22, p11);
	strassen_multiply(n, p11, a11, p_res, p_temp); 
}

void strassen_P4 (int n, matrix a22, 
				matrix b21, matrix b11
				matrix p)
//  P4 =  A22*(B21-B11)
{
	sub(n, b21, b11, p11);
	strassen_multiply(n, p11, a22, p_res, p_temp); 
}



void strassen_P5 (int n, matrix a11, matrix a12, 
				matrix b22,
				matrix p)
//  P5 = (A11 + A12)B22 
{

	add(n, a12, a12, p11);
	strassen_multiply(n, p11, b22, p_res, p_temp); //p1
}


void strassen_P6 (int n, matrix a21, matrix a11, 
				matrix b11, matrix b12
				matrix p)
//   P6 = (A21-A11)*(B11+B12) 
{

	add(n, b11, b12, p11);
	sub(n, a21, a11, p12)
	strassen_multiply(n, p11, p12, p_res, p_temp); 
}

void strassen_P7 (int n, matrix a12, matrix a22, 
				matrix b21, matrix b22
				matrix p)
//   P7 = (A12-A22)*(B21+B22)
{

	sub(n, a12, a22, p11)
	add(n, b21, b22, p12);
	strassen_multiply(n, p11, p12, p_res, p_temp); 
}



void strassenParallel(int n, double first[], double second[], double multiply[])
{
	printf("Running parallel strassenMultiplication\n");
	matrix a, b, c, d, P1, P2, P3, P4, P5, P6, P7;
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

	strassen_P1(n, a11, a22, b11, b22, P1);
	strassen_P2(n, a21, a22, b11, P2);
	strassen_P3(n, a11, b12, b22, P3);
	strassen_P4(n, a22, b21, b11, P4);
	strassen_P5(n, a11, a12, b22, P5);
	strassen_P6(n, a21, a11, b11, b12, P6);
	strassen_P7(n, a12, a22, b21, b22, P7);

	//  C11 = P1 + P4 - P5 + P7
	//  C22 = P1 - P2 + P3 + P6

	add(n, P1->[3], P4->[3], c11);
	sub(n, c11, P5->[3], c11);
	add(n, c11, P7->[3], c11);

	sub(n, P1->[3], P2->[3], c22);
	add(n, c22, P3->[3], c22);
	add(n, c22, P6->[3], c22);



}

