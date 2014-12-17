#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#define ndim  512
#define mdim  512

#define IDX(Y, X) (ndim * Y + X) //rows first

typedef enum { false, true } bool;

void multiplyPart(int sI, double first[], double second[], double output[])
{
  int x,y;
    for (x = 0; x < ndim; x++)
    {
      for (y = 0; y < mdim; y++)
        output[IDX(x,y)] += first[IDX(x,sI)] * second[IDX(sI,y)];
    }
}


bool isValid(double first[], double second[], double multiplied[])
{
    
    int c, d, k;
    double sum = 0;
    bool valid = true;
    //standard matrix multiplication
    for (c = 0; c < ndim; c++)
    {
      for (d = 0; d < mdim; d++)
      {
        for (k = 0; k < ndim; k++)
        {
          sum = sum + first[IDX(c,k)]*second[IDX(k,d)];
        }
 
        if (multiplied[IDX(c,d)] != sum)
        {
          valid = false;
          printf("result matrix not valid in row %d, col %d \n", c, d);
          break;
        }

        sum = 0;
      }
    }

    return valid;
}


int main()
{

  srand(time(NULL));


  int c, d, k, sI;
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

  clock_t end = clock();
  float seconds = (float)(end - start) / CLOCKS_PER_SEC;

  printf("matrix multiplication took: %f\n", seconds);

  if (isValid(first, second, multiply))
    printf("valid matrix multiplication\n");


  printf("finished \n");

  return 0;
}