#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h> //to call mkdir
#include <time.h>

#include "const.h"

int get_rand(int a,
             int b) // returns a random int between a <= b and b included.
{
    return a + rand() % (b + 1 - a);
}

void generate_test(FILE *file, int n, int rmax, int dmax)
{
    /*
    Generates n random tasks with deadlines <= dmax and stores them in file
    A bunch of tasks is stored as a text where the first line contains the
    number n of tasks, and the n next lines contain each two int
    representing the release time and the deadline
    */
    fprintf(file, "%d\n", n);
    int r, d; // release time and deadline
    for (int i = 0; i < n; i++) {
        r = get_rand(0, rmax);
        d = get_rand(r + D, max(dmax, r + D));
        fprintf(file, "%d %d\n", r, d);
    }
    fprintf(file, "\n");
}

void generate_testcases(FILE *file, int N, int n, int rmax, int dmax)
{
    /*
    A testcases file is stored as a file where the first line contains N the
    number of tasks groups followed by N descriptions of groups of n tasks each
    */
    fprintf(file, "%d\n\n", N);
    for (int i = 0; i < N; i++) {
        generate_test(file, n, rmax, dmax);
    }
}

int main()
{
    char name[100];
    srand((unsigned)time(NULL)); // initializes the seed
    mkdir("sched_tests", S_IRWXU);

    for (int n = 1; n <= nmax; n++) { // creates one file with Ntests groups of
                                      // n tasks for each value of n

        sprintf(name, "sched_tests/test_n=%d.in", n);
        FILE *file = fopen(name, "w+");
        int dmax = (int)((double)D * (double)(n + add_c));
        int rmax = (int)(fact_r * (double)dmax);
        generate_testcases(file, Ntests, n, rmax, dmax);
        fclose(file);
    }
    return 0;
}