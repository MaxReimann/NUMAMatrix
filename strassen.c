

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
* where the float indices refer to submatrices in an obvious way.
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
#include <xmmintrin.h>
#include "strassen.h"
#include "globals.h"

#include <assert.h>     /* assert */

#define IDX(Y, X) (n * Y + X) //rows first
void inline M4x4_SSE(int n, float *A, float *B, float *C);


/* c = a*b */
void strassen_multiply(int n, matrix a, matrix b, matrix c, matrix d)
{
	if (n <= BREAK) {
		float *p = a->d, *q = b->d, *r = c->d;
		//assert(n/4*4==n);


  const int NB =  32;

  float sum;

  for(int i=0; i<n; i+=NB)
    for(int j=0; j<n; j+=NB)
      for(int k=0; k<n; k+=NB)
      {
        int i_max = (i+NB < n)? i+NB : n;
        int j_max = (j+NB < n)? j+NB : n;
        int k_max = (k+NB < n)? k+NB : n;


			for(int i0=i; i0<i_max; i0+=4)
				for(int j0=j; j0<j_max; j0+=4)
				{
					//zero out 4x4 matrix from previous use
					__m128 zeroed = _mm_set1_ps(0.f);
					for (int h=0;h<4;h++)
						_mm_store_ps(&r[IDX((i0+h),j0)],zeroed); 
			    	
			    	for(int k0=k; k0<k_max; k0+=4)
						M4x4_SSE(n, &p[IDX(i0,k0)], &q[IDX(k0,j0)], &r[IDX(i0,j0)]);
			    }
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
void print_m128(__m128 var,char* text)
{
	float row[4] __attribute__((aligned(16)));
	assert(((size_t)&row[0])%16==0);
	_mm_store_ps(row, var);
    printf("%s: %f %f %f %f \n",text, row[0],row[1],row[2],row[3]);

}

void M4x4_SSE(int n, float * restrict A, float * restrict B, float * restrict C) {
    __m128 row1 = _mm_load_ps(&B[IDX(0,0)]);
    __m128 row2 = _mm_load_ps(&B[IDX(1,0)]);
    __m128 row3 = _mm_load_ps(&B[IDX(2,0)]);
    __m128 row4 = _mm_load_ps(&B[IDX(3,0)]);
    for(int i=0; i<4; i++) {
        __m128 brod1 = _mm_set1_ps(A[IDX(i,0)]); //Set all four words with the same value 
        __m128 brod2 = _mm_set1_ps(A[IDX(i,1)]);
        __m128 brod3 = _mm_set1_ps(A[IDX(i,2)]);
        __m128 brod4 = _mm_set1_ps(A[IDX(i,3)]);


   		
   		__m128 prod1 = _mm_mul_ps(brod1, row1);
   		__m128 prod2 = _mm_mul_ps(brod2, row2);
   		__m128 prod3 = _mm_mul_ps(brod3, row3);
   		__m128 prod4 = _mm_mul_ps(brod4, row4);


        __m128 row = _mm_add_ps(
                    _mm_add_ps(prod1, prod2),
					_mm_add_ps(prod3, prod4));

         __m128 res = _mm_load_ps(&C[IDX(i,0)]);
         res = _mm_add_ps(res, row);
        _mm_store_ps(&C[IDX(i,0)], res);
    }
}


/* c = a+b */
void inline add(int n, matrix a, matrix b, matrix c)
{
	if (n <= BREAK) {
		float *p = a->d, *q = b->d, *r = c->d;
		int i, j;
		

		for (i = 0; i < n; i++) {
			for (j = 0; j+4 <= n; j+=4) {
				 __m128 row1 = _mm_load_ps(&p[IDX(i,j)]);
				 __m128 row2 = _mm_load_ps(&q[IDX(i,j)]);
				 __m128 res = _mm_add_ps(row1, row2);
				 _mm_store_ps(&r[IDX(i,j)], res);
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
		float *p = a->d, *q = b->d, *r = c->d;
		int i, j;

		for (i = 0; i < n; i++) {
			for (j = 0; j+4 <= n; j+=4) {
				 __m128 row1 = _mm_load_ps(&p[IDX(i,j)]);
				 __m128 row2 = _mm_load_ps(&q[IDX(i,j)]);
				 __m128 res = _mm_sub_ps(row1, row2);
				 _mm_store_ps(&r[IDX(i,j)], res);
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