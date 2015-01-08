#include <stdio.h>
#include <stdlib.h>

long
procsuffix(const char *arg)
{
  char *suffix;
  long val = strtol(arg, &suffix, 10);
  switch (*suffix) {
  case 'k':
  case 'K':
    val <<= 10;
    break;
  case 'm':
  case 'M':
    val <<= 20;
    break;
  case 'g':
  case 'G':
    val <<= 30;
    break;
  }

  return val;
}

