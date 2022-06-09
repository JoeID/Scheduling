#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h> //to call mkdir
#include <time.h>

#include "const.h"
#include "scheduling.h"
#include "stack.h"

//#define DEBUG

double benchmark(Taskgroup *tg, int N, FILE *save, Stack (*f_zones)(Taskgroup),
                 int *(*f_schedule)(Taskgroup, Stack))
// benchmarks the algorithm using f_zones for part I and schedule for part II.
// Returns failrate
{
    clock_t start, end;
    int failed = 0;
    double elapsed = 0;
    for (int i = 0; i < N; i++) {

        /* benchmarks Part I */

        start = clock();
        Stack st = f_zones(tg[i]);

        /* benchmarks part II */

        int *schedule = f_schedule(tg[i], st);
        if (schedule) {
            free(schedule);
        }
        else {
            failed++;
        }
        end = clock();
        elapsed += (1000 * (double)(end - start));
    }
    fprintf(save, "%.6f ", elapsed / (N * CLOCKS_PER_SEC));
    // calculates average time and failure rate
    return ((double)failed) / ((double)N);
}

int main()
{
#ifndef DEBUG

    char name[100];
    mkdir("results", S_IRWXU);
    srand((unsigned)time(NULL)); // initializes the seed
    FILE *save =
        fopen("results/times.out",
              "w+"); // we will store the times of the codes in this file

    int N = 0;

    for (int n = 1; n <= nmax; n++) {

        /* opens the file, extracts the taskgroups and sorts them */

        sprintf(name, "sched_tests/test_n=%d.in", n);
        FILE *src = fopen(name, "r");
        Taskgroup *taskgroups = get_taskgroups(src, &N); // gets the taskgroups
        fclose(src);
        for (int i = 0; i < N; i++)
            qsort(taskgroups[i].tasks, taskgroups[i].n, sizeof(Task),
                  cmp_func_r_time);
        for (int i = 0; i < N; i++) // sets the number of each task to its
                                    // position in the sorted list
            for (int j = 0; j < taskgroups[i].n; j++)
                taskgroups[i].tasks[j].i = j;

        /* benchmarks the algorithms on the taskgroups */

        fprintf(save, "n=%d\n", n);
        printf("n=%d\n", n);

        // quadratic Part I and quadratic Part II
        double failrate1 = benchmark(taskgroups, N, save, f_zones_quadratic,
                                     schedule_quadratic);

        // quadratic Part I and quasi-linear Part II
        double failrate2 = benchmark(taskgroups, N, save, f_zones_quadratic,
                                     schedule_q_linear);

        // quasi-linear Part I and quadratic Part II
        //double failrate3 = benchmark(taskgroups, N, save, f_zones_q_linear, schedule_quadratic);

        // quasi-linear Part I and quasi-linear Part II
        //double failrate4 = benchmark(taskgroups, N, save, f_zones_q_linear, schedule_quadratic);

        assert(failrate1 == failrate2);
        //assert(failrate1 == failrate3);
        //assert(failrate1 == failrate4);
        fprintf(save, "0 0 %.3f\n", failrate1);

        /* frees the taskgroups */

        for (int i = 0; i < N; i++)
            free(taskgroups[i].tasks);
        free(taskgroups);
    }

    fclose(save);

    return 0;

#else

    int N = 0;
    FILE *src = fopen("sched_tests/test_example", "r");
    Taskgroup *taskgroups = get_taskgroups(src, &N); // gets the taskgroups
    fclose(src);
    qsort(taskgroups[0].tasks, taskgroups[0].n, sizeof(Task), cmp_func_r_time);
    for (int j = 0; j < taskgroups[0].n; j++)
        taskgroups[0].tasks[j].i = j;

    printf("Scheduling following tasks :\n");
    show_tasks(taskgroups[0]);
    Stack st = f_zones_quadratic(taskgroups[0]);
    printf("Forbidden regions : \n");
    show_stack(&st);
    int *schedule = schedule_quadratic(taskgroups[0], st);
    show_schedule(schedule, taskgroups[0].n);

    for (int i = 0; i < N; i++)
        free(taskgroups[i].tasks);
    free(taskgroups);
    free(schedule);
    return 0;

#endif
}