#include <unistd.h>

#ifndef CONST_SCHED
#define CONST_SCHED

#define add_c 7 // deadline max for generating n tasks is dmax = D * (n + add_c)
#define fact_r 0.95 // release time max for generating n tasks is fact_r * dmax
#define Ntests 1000 // number of tests
#define nmax 1000
#define D 2500 // duration of a task
#define P 17   // period 

#define true 1
#define false 0
typedef int bool;

#define max(a, b) (((a) >= (b)) ? (a) : (b))
#define min(a, b) (((a) <= (b)) ? (a) : (b))
#define abs(a) (((a) < 0) ? -(a) : (a))

#define Nproc 1 //sysconf(_SC_NPROCESSORS_ONLN) // number of logical cores

#endif