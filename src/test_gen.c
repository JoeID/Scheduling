#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h> //to call mkdir
#include <time.h>

#include "const.h"

int max(int a, int b) { return (a >= b) ? a : b; }

int min(int a, int b) { return (a <= b) ? a : b; }

int get_rand(int a, int b) // returns a random number between a and b included
{
    return a + rand() % (b + 1 - a);
}

void generate_test(FILE *file, int n, int dmax)
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
        r = get_rand(0, dmax - 1);
        d = get_rand(r + 1, dmax);
        fprintf(file, "%d %d\n", r, d);
    }
    fprintf(file, "\n");
}

void generate_testcases(FILE *file, int N, int n, int dmax)
{
    /*
    A testcases file is stored as a file where the first line contains N the
    number of groups of tasks (i.e testcases) and the rest of the file contains
    N descriptions of groups of n tasks each
    */
    fprintf(file, "%d\n\n", N);
    for (int i = 0; i < N; i++) {
        generate_test(file, n, dmax);
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
        generate_testcases(file, Ntests, n, D * (int)(fact * (double)n));
        fclose(file);
    }
    return 0;
}