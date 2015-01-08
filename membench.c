#define _GNU_SOURCE
#include "membench.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sched.h>
#include <numa.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>
#include <assert.h>
#include "util.h"


void
parsearg()
{
  int opt;

  option.usecore = 1;
  option.access_size = 1 << 20;
  option.timeout = 2; // * 1000 * 1000.0; // default timeout is 10 seconds

  // while ((opt = getopt(argc, argv, "c:s:t:")) != -1) {
  //   switch (opt) {
  //   case 'c':
  //     option.usecore = atoi(optarg);
  //     break;
  //   case 's':
  //     option.access_size = procsuffix(optarg);
  //     break;
  //   case 't':
  //     option.timeout = atof(optarg); // * 1000000.0;
  //     break;
  //   default:
  //     fprintf(stderr, "Usage : %s [-c cpuno] [-s accesssize] [-t timeout(sec)]\n", argv[0]);
  //     exit(EXIT_FAILURE);
  //   }
  // }
}

static inline uint64_t
read_tsc(void)
{
    uintptr_t ret;
    uint32_t eax, edx;
    __asm__ __volatile__("cpuid; rdtsc;"
                         : "=a" (eax) , "=d" (edx)
                         :
                         : "%ebx", "%ecx");
    ret = ((uint64_t)edx) << 32 | eax;
    return ret;
}

static inline void
swap_long(long *ptr1, long *ptr2)
{
    long tmp;
    if (ptr1 != ptr2) {
        tmp = *ptr1;
        *ptr1 = *ptr2;
        *ptr2 = tmp;
    }
}

void
numa_membench(mem_bench_info_t *mbinfo)
{

  assert(mbinfo->destnode <= numa_max_node());

  {
    long size, freep;
    size = numa_node_size(mbinfo->destnode, &freep);
    //printf("node %d : total = %ld(B), free = %ld(B)\n", mbinfo->destnode, size, freep);
    assert(freep >= mbinfo->working_size);

    mbinfo->working_area =
      (long *)numa_alloc_onnode(mbinfo->working_size, mbinfo->destnode);
    if (NULL == mbinfo->working_area) {
      perror("numa_alloc_onnode");
      exit(EXIT_FAILURE);
    }
    memset(mbinfo->working_area, 0, mbinfo->working_size);
  }

  // memory_stress_rand(&mbinfo->pc, mbinfo->working_area, mbinfo->working_size);

  // release resources
  numa_free(mbinfo->working_area, mbinfo->working_size);
}

int
notMain()
{
  int i;
  cpu_set_t cpuset;
  mem_bench_info_t mbinfo;

  if (numa_available() == -1){
    fprintf(stderr, "numa functions aren't available\n");
    exit(EXIT_FAILURE);
  }

  parsearg();
  mbinfo.working_size = option.access_size;

  // set affinity
  CPU_ZERO(&cpuset);
  CPU_SET(option.usecore, &cpuset);
  sched_setaffinity(getpid(), sizeof(cpu_set_t), &cpuset);

  // read benchmark
  printf("===========================================\n"
         "memory benchmark\n"
         "===========================================\n");
  for (i = 0; i <= numa_max_node(); i++) {
    mbinfo.destnode = i;
    mbinfo.pc.ops = 0;
    mbinfo.pc.clk = 0;
    printf("starting benchmark...\n");
    numa_membench(&mbinfo);
    printf("memory_alloc_node\t%d\n"
           "access_size\t%ld\n"
           "total_ops\t%ld\n"
           "total_clk\t%ld\n"
           "exec_time_sec\t%.9f\n"
           "ops_per_sec\t%e\n"
           "clk_per_op\t%e\n"
           "usec_per_op\t%f\n",
           i,
           mbinfo.working_size,
           mbinfo.pc.ops,
           mbinfo.pc.clk,
           mbinfo.pc.wallclocktime,
           mbinfo.pc.ops / mbinfo.pc.wallclocktime,
           ((double)mbinfo.pc.clk) / mbinfo.pc.ops,
           mbinfo.pc.wallclocktime * 1000000 / mbinfo.pc.ops
           );
  }

  return 0;
}
