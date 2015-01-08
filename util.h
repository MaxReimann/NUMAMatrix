#ifndef __SCHEME_IOMICROBENCH__
#define __SCHEME_IOMICROBENCH__

#include <stdio.h>
#include <time.h>

#define BLOCK_SIZE 512
#define PRNG_BUFSZ 64

#define CACHELINE_SIZE 64 // assume cache line is 64B

#define TS2SEC(ts) (((double) (ts).tv_sec) + ((double) (ts).tv_nsec * 1e-09))
#define TIMEINTERVAL_SEC(sts, fts)                                      \
  ((fts.tv_sec - sts.tv_sec) + (fts.tv_nsec - sts.tv_nsec) * 1e-09)
#define CLOCK_GETTIME(ts_ptr)                                    \
  {                                                              \
    if (clock_gettime(CLOCK_REALTIME, ts_ptr) != 0) {            \
      perror("clock_gettime(3) failed ");                        \
      fprintf(stderr, " @%s:%d\n", __FILE__, __LINE__);          \
    }                                                            \
  }

long procsuffix(char *);

#endif // __SCHEME_IOMICROBENCH__
