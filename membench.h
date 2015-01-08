#pragma once

typedef struct {
    unsigned long ops;
    unsigned long clk;
    double wallclocktime;
} perf_counter_t;

typedef struct {
  perf_counter_t pc;
  int destnode;
  long *working_area;
  long working_size;
} mem_bench_info_t;

struct {
  int usecore;
  long access_size;
  double timeout;
} option;

void numa_membench(mem_bench_info_t *mbinfo);
int notMain();