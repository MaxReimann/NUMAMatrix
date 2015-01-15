
/*
* strassen.c
*
* Courtesy [Buhler 1993].
* Routines to realize the Strassen recursive matrix multiplication.
* Multiply n by n matrices a and b, putting the result in c, and
* using the matrix d as scratch space.  The Strassen algorithm is:
*
*      q7 = (a12-a22)(b21+b22)
*      q6 = (-a11+a21)(b11+b12)
*      q5 = (a11+a12)b22
*      q4 = a22(-b11+b21)
*      q3 = a11(b12-b22)
*      q2 = (a21+a22)b11
*      q1 = (a11+a22)(b11+b22)
*      c11 = q1+q4-q5+q7
*      c12 = q3+q5
*      c21 = q2+q4
*      c22 = q1+q3-q2+q6
*
* where the double indices refer to submatrices in an obvious way.
* Each line of multiply() that recursively calls itself computes one
* of the q's.  Four scratch half-size matrices are required by the
* sequence of computations here; with some rearrangement this
* storage requirement can be reduced to three half-size matrices.
*
* The small matrix computations (i.e., for n <= BREAK) can be
* optimized considerably from those given here; in particular, this
* is important to do before the value of BREAK is chosen optimally.
*
*/
#include "strassen.h"

/* c = a*b */
void strassen_multiply(int n, matrix a, matrix b, matrix c, matrix d)
{
	if (n <= BREAK) {
		double sum, **p = a->d, **q = b->d, **r = c->d;
		int i, j, k;

		for(int i=0; i<n; i++)
		    for(int j=0; j<n; j++)
		    {
		      sum = 0;
		      for(int k=0; k<n; k++)
	                sum += p[i][k] * q[k][j];
	           r[i][j] = sum;
	       }
	}
	else {
		n /= 2;
		sub(n, a12, a22, d11);
		add(n, b21, b22, d12);
		strassen_multiply(n, d11, d12, c11, d21); //p7
		sub(n, a21, a11, d11);
		add(n, b11, b12, d12);
		strassen_multiply(n, d11, d12, c22, d21);//p6
		add(n, a11, a12, d11);
		strassen_multiply(n, d11, b22, c12, d12);//p5
		sub(n, c11, c12, c11);
		sub(n, b21, b11, d11);
		strassen_multiply(n, a22, d11, c21, d12);//p4
		add(n, c21, c11, c11);
		sub(n, b12, b22, d11);
		strassen_multiply(n, a11, d11, d12, d21);//p3
		add(n, d12, c12, c12); //p3 + p5
		add(n, d12, c22, c22); //p3 + p6
		add(n, a21, a22, d11);
		strassen_multiply(n, d11, b11, d12, d21);//p2
		add(n, d12, c21, c21); //p2 + p4
		sub(n, c22, d12, c22); //p6 +
		add(n, a11, a22, d11);
		add(n, b11, b22, d12);
		strassen_multiply(n, d11, d12, d21, d22); //p1
		add(n, d21, c11, c11);
		add(n, d21, c22, c22);
	}
}



/* c = a+b */
void inline add(int n, matrix a, matrix b, matrix c)
{
	if (n <= BREAK) {
		double **p = a->d, **q = b->d, **r = c->d;
		int i, j;

		for (i = 0; i < n; i++) {
			for (j = 0; j < n; j++) {
				r[i][j] = p[i][j] + q[i][j];
			}
		}
	}
	else {
		n /= 2;
		add(n, a11, b11, c11);
		add(n, a12, b12, c12);
		add(n, a21, b21, c21);
		add(n, a22, b22, c22);
	}
}

/* c = a-b */
void inline sub(int n, matrix a, matrix b, matrix c)
{
	if (n <= BREAK) {
		double **p = a->d, **q = b->d, **r = c->d;
		int i, j;

		for (i = 0; i < n; i++) {
			for (j = 0; j < n; j++) {
				r[i][j] = p[i][j] - q[i][j];
			}
		}
	}
	else {
		n /= 2;
		sub(n, a11, b11, c11);
		sub(n, a12, b12, c12);
		sub(n, a21, b21, c21);
		sub(n, a22, b22, c22);
	}

}
