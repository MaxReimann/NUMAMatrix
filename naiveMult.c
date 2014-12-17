#include <stdio.h>
#include <time.h>
#include <stdlib.h>

const int ndim = 1000;
const int mdim = 1000;

int main()
{

  srand(time(NULL));


  int c, d, k;
  long sum = 0;
  int (*first)[ndim]=malloc(ndim*mdim*sizeof(int));
  int (*second)[ndim]=malloc(ndim*mdim*sizeof(int));
  int (*multiply)[ndim]=malloc(ndim*mdim*sizeof(int));
  //  int **first, second[ndim][mdim], multiply[ndim][mdim];


  for (c = 0 ; c < ndim; c++)
  {
    for (d = 0 ; d < mdim; d++)
    {
      first[c][d] = rand() % 100; //int between 0 - 1000
      second[c][d] = rand() % 100;
    }
  
  }

  clock_t start = clock();

    for ( c = 0 ; c < ndim ; c++ )
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
    }

  clock_t end = clock();
  float seconds = (float)(end - start) / CLOCKS_PER_SEC;

  printf("matrix multiplication took: %f", seconds);
 
    // for ( c = 0 ; c < ndim ; c++ )
    // {
    //   for ( d = 0 ; d < mdim ; d++ )
    //   {
    //     printf("%d\t", multiply[c][d]);
    //   }
 
    //   printf("\n");
    // }
    printf("\n finished \n");

  return 0;
}