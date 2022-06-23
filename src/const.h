#include <unistd.h>

#ifndef CONST_SCHED
#define CONST_SCHED

#define add_c                                                                  \
    10 // deadline max for generating n tasks is dmax = D * (n + add_c)
#define fact_r 0.95  // release time max for generating n tasks is fact_r * dmax
#define Ntests 100 // number of tests
#define nmax 100
#define D 5 // duration of a task

#define true 1
#define false 0
typedef int bool;

#define max(a, b) (((a) >= (b)) ? (a) : (b))
#define min(a, b) (((a) <= (b)) ? (a) : (b))

#define Nproc sysconf(_SC_NPROCESSORS_ONLN) // number of logical cores

#endif