#ifndef CONST_SCHED
#define CONST_SCHED

#define fact_d 1.2    // the deadline max for generating n tasks is dmax = D * fact_d * n
#define fact_r 0.8     // the release time max for generating n tasks is c * dmax
#define Ntests 500 // number of tests
#define nmax 100
#define D 5 // duration of a task

#define true 1
#define false 0
typedef int bool;

#define max(a,b) (((a) >= (b)) ? (a) : (b))
#define min(a,b) (((a) <= (b)) ? (a) : (b))

#endif