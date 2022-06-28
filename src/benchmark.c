#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h> //to call mkdir
#include <time.h>
#include <string.h>

#include "const.h"
#include "scheduling.h"
#include "scheduling_mael.h"
#include "stack.h"
#include "test_gen.h"

//#define DEBUG

struct arg_ret { // to pass arguments to pthreads and get return values
    Taskgroup *tg;
    int N;
    Stack (*f_zones)(Taskgroup);
    int *(*f_schedule)(Taskgroup, Stack);
    double elapsed;
    int failed;
};
typedef struct arg_ret Arg_ret;

bool OTF = false; // true if we generate on the fly, false otherwise

void benchmark(Arg_ret *arg_ret)
// benchmarks the algorithm using f_zones for part I and schedule for part II.
// Returns failrate
{
    clock_t start, end;
    int failed = 0;
    double elapsed = 0;
    for (int i = 0; i < arg_ret->N; i++) {

        start = clock();

        // try to greedily solve the pb, call the real algorithm if it fails
        int *schedule = schedule_greedy(arg_ret->tg[i]);
        if (schedule) {
            assert(is_valid(arg_ret->tg[i], schedule));
            free(schedule);
        }
        else {
            Stack st = arg_ret->f_zones(arg_ret->tg[i]);

            int *schedule = arg_ret->f_schedule(arg_ret->tg[i], st);
            free_stack(&st);
            if (schedule) {
                assert(is_valid(arg_ret->tg[i], schedule));
                free(schedule);
            }
            else {
                failed++;
            }
        }

        end = clock();
        elapsed += (1000 * (double)(end - start));
    }
    // calculates average time and failure rate
    arg_ret->elapsed = elapsed / (arg_ret->N * CLOCKS_PER_SEC);
    arg_ret->failed = failed;
}
/*
void benchmark_mael(Taskgroup *tg, int N, FILE *save)
{
    clock_t start, end;
    double elapsed = 0;
    Route *elems;
    Ensemble *ens;

    for (int i = 0; i < N; i++) {

        start = clock();

        elems = init_element(); // Filling the chained list with tasks
        for (int j = 0; j < tg->n; j++)
            elems = ajoute_elemt(elems, j, tg[i].tasks[j].release_time,
                                 tg[i].tasks[j].deadline);

        ens = algo_simons(elems, tg[i].n, D, 0, 0); // Simons' algo

        if (ens)
            libereens(ens);
        freeelems(elems);

        end = clock();
        elapsed += (1000 * (double)(end - start));
    }
    // calculates average time and failure rate
    fprintf(save, "%.6f ", elapsed / (N * CLOCKS_PER_SEC));
}
*/
void *benchmark_splitted(void *args)
{
    benchmark((Arg_ret *)args);
    return NULL;
}

void wrapper_fun(Taskgroup tg)
// tg.tasks an array of tasks which you can get the size by using tg.n
// this function doesn't free tg.tasks
{
    qsort(tg.tasks, tg.n, sizeof(Task), cmp_func_r_time);

    printf("Scheduling following tasks :\n");
    show_tasks(tg);
    Stack st = f_zones_quadratic(tg);
    printf("Forbidden regions are : \n");
    show_stack(&st);
    int *schedule = schedule_q_linear(tg, st);
    free_stack(&st);
    if (schedule) {
        printf("A solution is :\n");
        show_schedule(schedule, tg.n);
        printf("Effective time : %d\n", effective_time(schedule, tg.n));
        free(schedule);
    }
    else {
        printf("Couldn't schedule\n");
    }
}

int main(int argc, char *argv[])
{
#ifndef DEBUG
    if(argc == 2)
        if(!strcmp(argv[1], "-otf"))
            OTF = true;
    if (!OTF) {
        printf("Generating...\n");
        test_gen();
    }
    printf("Benchmarking on %ld threads...\n", Nproc);
    char name[100];
    pthread_t threads[Nproc];
    mkdir("results", S_IRWXU);
    srand((unsigned)time(NULL)); // initializes the seed
    FILE *save = fopen("results/times.out", "w+");
    fprintf(save, "%d %d %.2f\n", D, add_c, fact_r);

    for (int n = 1; n <= nmax; n++) {
        int N = 0, failed1 = 0, failed2 = 0, failed3 = 0, failed4 = 0;

        /* opens the file, extracts the taskgroups and sorts them */

        Taskgroup *taskgroups;
        if (!OTF) {
            sprintf(name, "sched_tests/test_n=%d.in", n);
            FILE *src = fopen(name, "r");
            taskgroups = get_taskgroups(src, &N);
            fclose(src);
        }
        else {
            taskgroups = get_generate_testcases(Ntests, n);
            N = Ntests;
        }
        double elapsed = 0;
        for (int i = 0; i < N; i++)
            qsort(taskgroups[i].tasks, taskgroups[i].n, sizeof(Task),
                  cmp_func_r_time);

        /* benchmarks the algorithms on the taskgroups */

        fprintf(save, "n=%d\n", n);
        printf("n=%d\n", n);

        Arg_ret *args = (Arg_ret *)malloc(Nproc * sizeof(Arg_ret));
        for (int i = 0; i < Nproc;
             i++) { // suppose that N is a multiple of Nproc
            args[i].f_zones = f_zones_quadratic;
            args[i].N = N / Nproc;
            args[i].tg = taskgroups + i * N / Nproc;
        }
        args[Nproc - 1].N = N - (Nproc - 1) * N / Nproc;
        args[Nproc - 1].tg = taskgroups + N - args[Nproc - 1].N;

        // quadratic Part I and quadratic Part II
        for (int i = 0; i < Nproc; i++)
            args[i].f_schedule = schedule_quadratic;
        for (int i = 0; i < Nproc; i++)
            pthread_create(threads + i, NULL, benchmark_splitted, args + i);
        for (int i = 0; i < Nproc; i++)
            pthread_join(threads[i], NULL);
        for (int i = 0; i < Nproc; i++) {
            failed1 += args[i].failed;
            elapsed += args[i].elapsed;
        }
        fprintf(save, "%.3f ", elapsed);
        elapsed = 0;

        // quadratic Part I and quasi-linear Part II
        for (int i = 0; i < Nproc; i++)
            args[i].f_schedule = schedule_q_linear;
        for (int i = 0; i < Nproc; i++)
            pthread_create(threads + i, NULL, benchmark_splitted, args + i);
        for (int i = 0; i < Nproc; i++)
            pthread_join(threads[i], NULL);
        for (int i = 0; i < Nproc; i++) {
            failed2 += args[i].failed;
            elapsed += args[i].elapsed;
        }
        fprintf(save, "%.3f ", elapsed);
        elapsed = 0;

        // quasi-linear Part I and quadratic Part II
        // double failrate3 = benchmark(taskgroups, N, save, f_zones_q_linear,
        // schedule_quadratic);

        // quasi-linear Part I and quasi-linear Part II
        // double failrate4 = benchmark(taskgroups, N, save, f_zones_q_linear,
        // schedule_quadratic);
        fprintf(save, "0 0 0 ");

        // Mael's implementation of Simons' algorithm
        // benchmark_mael(taskgroups, N, save);

        assert(failed1 == failed2);
        // assert(failed1 == failed3);
        // assert(failed1 == failed4);
        fprintf(save, "%.3f\n", ((double)failed1) / N);

        /* frees the taskgroups */

        free_taskgroups(taskgroups, N);
        free(args);
    }

    fclose(save);
#else
    // for debug purposes
    int N = 0;
    FILE *src = fopen("sched_tests/debug", "r");
    Taskgroup *taskgroups = get_taskgroups(src, &N); // gets the taskgroups
    fclose(src);
    qsort(taskgroups[0].tasks, taskgroups[0].n, sizeof(Task), cmp_func_r_time);

    wrapper_fun(taskgroups[0]);
#endif
    return 0;
}