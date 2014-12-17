#include <stdio.h>
#include <time.h>
#include <stdlib.h>


int main()
{

  srand(time(NULL));
  const int ndim = 10;
  const int mdim = 10;


  int c, d, k;
  long sum = 0;
  int first[ndim][mdim], second[ndim][mdim], multiply[ndim][mdim];


  for (c = 0 ; c < ndim; c++)
  {
    for (d = 0 ; d < mdim; d++)
    {
      first[c][d] = rand() % 100; //int between 0 - 1000
      second[c][d] = rand() % 100;
    }
  
  }


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

 
    for ( c = 0 ; c < ndim ; c++ )
    {
      for ( d = 0 ; d < mdim ; d++ )
      {
        printf("%d\t", multiply[c][d]);
      }
 
      printf("\n");
    }
    printf("finished \n");

  return 0;
}