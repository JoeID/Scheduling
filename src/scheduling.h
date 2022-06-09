#ifndef SCHED_SCHED
#define SCHED_SCHED

#include "stack.h"

typedef struct Task {
    int release_time;
    int deadline;
    int i; // the number of the task
} Task;

typedef struct Taskgroup { // struct to store a group of n tasks to schedule
    Task *tasks;
    int n;
} Taskgroup;

void show_tasks(Taskgroup tg);
void show_schedule(int *schedule, int n);
int cmp_func_r_time(const void *a, const void *b);
Stack f_zones_quadratic(Taskgroup tg);
Stack f_zones_q_linear(Taskgroup tg);
int *schedule_quadratic(Taskgroup tg, Stack st);
int *schedule_q_linear(Taskgroup tg, Stack st);
Taskgroup *get_taskgroups(FILE *file, int *N);

#endif