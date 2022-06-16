#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h> //to call mkdir
#include <time.h>

#include "const.h"
#include "scheduling.h"
#include "scheduling_mael.h"
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
        free_stack(&st);
        if (schedule) {
            free(schedule);
        }
        else {
            failed++;
        }
        end = clock();
        elapsed += (1000 * (double)(end - start));
    }
    // calculates average time and failure rate
    fprintf(save, "%.6f ", elapsed / (N * CLOCKS_PER_SEC));
    return ((double)failed) / ((double)N);
}

double benchmark_mael(Taskgroup *tg, int N, FILE *save)
{
    clock_t start, end;
    int failed = 0;
    double elapsed = 0;
    Route *elems;
    Ensemble *ens;

    for(int i = 0; i < N; i++) {
        
        start = clock();

        elems = init_element(); // Filling the chained list with tasks
        for (int j = 0; j < tg->n; j++)
            elems = ajoute_elemt(elems, j, tg[i].tasks[j].release_time, tg[i].tasks[j].deadline);

        // printf("Les jobs sont : \n");
        // affichejobs(elems);
        ens = algo_simons(elems, tg[i].n, D, 0, 0); // Simons' algo
        // printf("Le scheduling est : \n");
        // affiche_ensemble(ens);
        // printf("\n");

        if(ens == NULL) {
            failed ++;
        }
        else {
            libereens(ens);
        }

        end = clock();
        elapsed += (1000 * (double)(end - start));

        freeelems(elems);
    }
    // calculates average time and failure rate
    fprintf(save, "%.6f ", elapsed / (N * CLOCKS_PER_SEC));
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
    fprintf(save, "%.2f %.2f\n", fact_d, fact_r);

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
        // double failrate3 = benchmark(taskgroups, N, save, f_zones_q_linear,
        // schedule_quadratic);

        // quasi-linear Part I and quasi-linear Part II
        // double failrate4 = benchmark(taskgroups, N, save, f_zones_q_linear,
        // schedule_quadratic);
        fprintf(save, "0 0 ");
    
        // Mael's implementation of Simons' algorithm
        double failrate5 = benchmark_mael(taskgroups, N, save);
        
        assert(failrate1 == failrate2);
        // assert(failrate1 == failrate3);
        // assert(failrate1 == failrate4);
        // assert(failrate1 == failrate5);
        fprintf(save, "%.3f\n", failrate1);

        /* frees the taskgroups */

        for (int i = 0; i < N; i++)
            free(taskgroups[i].tasks);
        free(taskgroups);
    }

    fclose(save);

    return 0;

#else

    int N = 0;
    FILE *src = fopen("sched_tests/debug", "r");
    Taskgroup *taskgroups = get_taskgroups(src, &N); // gets the taskgroups
    fclose(src);
    qsort(taskgroups[0].tasks, taskgroups[0].n, sizeof(Task), cmp_func_r_time);
    for (int j = 0; j < taskgroups[0].n; j++)
        taskgroups[0].tasks[j].i = j;

    printf("Scheduling following tasks :\n");
    show_tasks(taskgroups[0]);
    Stack st = f_zones_quadratic(taskgroups[0]);
    //printf("Forbidden regions : \n");
    //show_stack(&st);
    int *schedule = schedule_quadratic(taskgroups[0], st);
    if(schedule){
        printf("A solution is :\n");
        show_schedule(schedule, taskgroups[0].n);
    }else{
        printf("Couldn't schedule\n");
    }
    printf("Effective time : %d\n", effective_time(schedule, taskgroups[0].n));

    for (int i = 0; i < N; i++)
        free(taskgroups[i].tasks);
    free(taskgroups);
    free(schedule);
    return 0;

#endif
}