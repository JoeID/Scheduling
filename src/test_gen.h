#ifndef TEST_GEN_SCHED
#define TEST_GEN_SCHED

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h> //to call mkdir
#include <time.h>

#include "scheduling.h"

int get_rand(int a, int b);
void generate_test(FILE *file, int n, int rmax, int dmax);
void generate_testcases(FILE *file, int N, int n, int rmax, int dmax);
void test_gen();
Taskgroup *get_generate_testcases(int N, int n);

#endif