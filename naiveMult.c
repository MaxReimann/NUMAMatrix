#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#define ndim  256
#define mdim  256

#define IDX(Y, X) (ndim * Y + X) //rows first

void multiplyPart(int sI, double first[], double second[], double output[])
{
  int x,y;
    for (x = 0; x < ndim; x++ )
    {
      for (y = 0; y < mdim; y++)
        output[IDX(x,y)] += first[IDX(x,sI)] * second[IDX(sI,y)];
    }
}




int main()
{

  srand(time(NULL));


  int c, d, k, sI;
  long sum = 0;
  double *first=malloc(ndim*mdim*sizeof(double));
  double *second=malloc(ndim*mdim*sizeof(double));
  double *multiply=malloc(ndim*mdim*sizeof(double));
  //  int **first, second[ndim][mdim], multiply[ndim][mdim];

  for (c = 0 ; c < ndim; c++)
  {
    for (d = 0 ; d < mdim; d++)
    {
      first[IDX(c,d)] = rand() % 100; //int between 0 - 1000
      second[IDX(c,d)] = rand() % 100;
    }
  
  }
  clock_t start = clock();

  for (sI=0; sI < ndim; sI++)
  {
    multiplyPart(sI,first,second,multiply);
  }

/*    for ( c = 0 ; c < ndim ; c++ )
    {
      for ( d = 0 ; d < mdim ; d++ )
      {
        for ( k = 0 ; k < ndim ; k++ )
        {
          sum = sum + first[c][k]*second[k][d];
        }
 
        multiply[c][d] = sum;
        sum = 0;
      }
    }*/

  clock_t end = clock();
  float seconds = (float)(end - start) / CLOCKS_PER_SEC;

  printf("matrix multiplication took: %f", seconds);
  printf("\n finished \n");

  return 0;
}