#include <stdio.h>
#include <stdlib.h>

#include "const.h"
#include "heap.h"
#include "scheduling.h"
#include "stack.h"
#include "taskload_tree.h"
#define OUTPUT false

// comparison function to sort the tasks by growing release time, and
// growing deadlines if two release times are equal
int cmp_func_r_time(const void *a, const void *b)
{
    Task T1 = *(Task *)a;
    Task T2 = *(Task *)b;
    if (T1.release_time == T2.release_time)
        return T1.deadline - T2.deadline;
    return T1.release_time - T2.release_time;
}

void show_tasks(Taskgroup tg)
{
    for (int i = 0; i < tg.n; i++)
        printf("Task %d(%d, %d) ", tg.tasks[i].i, tg.tasks[i].release_time,
               tg.tasks[i].deadline);
    printf("\n");
}

void show_schedule(int *schedule, int n)
{
    for (int i = 0; i < n; i++)
        printf("Task %d : %d | ", i, schedule[i]);
    printf("\n");
}

// extracts a group of tasks to schedule from the file
Taskgroup get_tasks(FILE *file)
{
    int n;
    fscanf(file, "%d\n", &n);
    Task *task_arr = (Task *)malloc(n * sizeof(Task));
    for (int i = 0; i < n; i++) {
        fscanf(file, "%d %d\n", &task_arr[i].release_time,
               &task_arr[i].deadline);
    }
    return (Taskgroup){.n = n, .tasks = task_arr};
}

// extracts the taskgroups inside the file and puts its count in N
Taskgroup *get_taskgroups(FILE *file, int *N)
{
    fscanf(file, "%d\n", N);
    Taskgroup *taskgroups = (Taskgroup *)malloc((*N) * sizeof(Taskgroup));
    for (int i = 0; i < (*N); i++) {
        fscanf(file, "\n");
        taskgroups[i] = get_tasks(file);
        fscanf(file, "\n");
    }
    return taskgroups;
}

void show_c_times(int *c_times, int n)
{
    printf("Critical times : \n");
    for (int i = 0; i < n; i++) {
        printf(" | %d : %d", i, c_times[i]);
    }
    printf("\n\n");
}

Stack f_zones_quadratic(Taskgroup tg)
// Calculates the forbidden zones in O(n²) time. We suppose the tasks are sorted
// by growing release times, and growing deadlines if equality
{
    int *c_times = (int *)malloc(tg.n * sizeof(int)); // critical times
    for (int i = 0; i < tg.n; i++)
        c_times[i] = tg.tasks[i].deadline; // c_times[i] == tg.tasks[i].deadline
                                           // <=> c_times[i] is not defined
    Stack st = create_stack(tg.n);         // stack of forbidden regions

    /*CALCULATE THE FORBIDDEN REGIONS*/

    for (int i = tg.n - 1; i >= 0; i--) {
        int ri = tg.tasks[i].release_time, di = tg.tasks[i].deadline;
        // updates the critical times
        for (int j = tg.n - 1; j >= 0; j--) {
            int dj = tg.tasks[j].deadline;
            if (dj >= di) {
                c_times[j] -= D;
                // if c_times[j] is in a forbidden region, set it just below
                for (int k = st.top; k >= 0; k--) { 
                    if (st.items[k].start < c_times[j] &&
                        c_times[j] < st.items[k].end)
                        c_times[j] = st.items[k].start;
                    if(st.items[k].start > c_times[j])
                        break;
                }
            }
        }

        // updates the forbidden regions
        if (i == 0 || tg.tasks[i - 1].release_time < ri) {

            // sets c to the minimal of defined critical times
            int c = __INT32_MAX__;
            for (int i = 0; i < tg.n; i++)
                // if c_times[i] is defined
                if (c_times[i] != tg.tasks[i].deadline)
                    c = min(c, c_times[i]);

            if (c < ri) {
                if (OUTPUT)
                    printf("No schedule is possible\n");
                // empty the stack
                st.items = NULL;
                st.top = -1;
                free(c_times);
                return st;
            }
            // declaration of a new forbidden region
            if (ri <= c && c < ri + D) {
                push_st(&st, (Interval){.start = c - D, .end = ri});
                merge_last_st(&st);
            }
        }
    }
    free(c_times);
    return st;
}

Stack f_zones_q_linear(Taskgroup tg)
// Calculates the forbidden zones in O(nlogn) time. We suppose the tasks are
// sorted by growing release times, and growing deadlines if equality
{
    return (Stack){
        .items = NULL, .maxsize = 0, .top = -1}; // returns an empty stack
}

int *schedule_quadratic(Taskgroup tg, Stack st)
{
    // schedules the tasks if possible (all have a duration of D) in O(n²)
    // complexity by a greedy approach. Returns NULL or a malloced int*. Does
    // not free the stack

    if (!st.items)
        return NULL;

    int *schedule = (int *)malloc(tg.n * sizeof(int)), t = 0;
    for (int i = 0; i < tg.n; i++)
        schedule[i] = -1; // schedule[i] == -1 <=> tg.tasks[i] in unscheduled
    /* s is the index of the greatest sup of forbidden zones lower than t, i.e s
    is the min int such as st.items[s].end < t
    remember that st.items is an array of disjoint intervals ordered by
    decreasing inf */
    int s = st.top;
    while (true) {

        // the minimum release time of unscheduled tasks ready at t
        int min_ready_t = __INT32_MAX__;
        for (int j = 0; j < tg.n; j++) {
            if (schedule[j] < 0 && tg.tasks[j].release_time <= t) {
                min_ready_t = min(min_ready_t, tg.tasks[j].release_time);
                break;
            }
        }

        // if no unscheduled task is ready at t, sets i to the min of release
        // times of unscheduled tasks
        if (min_ready_t == __INT32_MAX__) {
            t = __INT32_MAX__;
            for (int j = 0; j < tg.n; j++)
                if (schedule[j] < 0)
                    t = min(t, tg.tasks[j].release_time);
        }
        if (t == __INT32_MAX__) // no task to schedule
            break;

        while (s >= 0 && st.items[s].end < t) { // updates s
            s--;
        }
        if (s >= 0 && st.items[s].start < t &&
            t < st.items[s].end) { // if t is in a forbidden zone
            t = st.items[s].end;
            if (s < st.top)
                s++;
        }

        int min_deadline_t = __INT32_MAX__; // the minimum deadline of
                                            // unscheduled tasks ready at t
        int j0 = -1; // the index of the task that has the minimum deadline
                     // among those ready at t

        for (int j = 0; j < tg.n; j++) {
            if (schedule[j] < 0 && tg.tasks[j].release_time <= t) {
                if (tg.tasks[j].deadline < min_deadline_t) {
                    min_deadline_t = tg.tasks[j].deadline;
                    j0 = j;
                }
            }
        }
        if (j0 == -1) // no task to schedule
            break;
        schedule[j0] = t;
        t += D;
    }

    return schedule;
}

int effective_time(int *schedule, int n)
// returns the effective time of the schedule
{
    if (!schedule)
        return -1;
    int s_min = __INT32_MAX__, s_max = 0;
    for (int i = 0; i < n; i++) {
        s_min = min(s_min, schedule[i]);
        s_max = max(s_max, schedule[i]);
    }
    return s_max + D - s_min;
}

int *schedule_q_linear(Taskgroup tg, Stack st)
{
    // schedules the tasks if possible (all have a duration of D) in O(nlogn)
    // complexity by a greedy approach. returns NULL or a malloced int*. Does
    // not free the stack

    if (!st.items) // means that a contradiction has been discovered in the
                   // forbidden zones algorithm
        return NULL;

    // tg.tasks is sorted by growing release time
    TaskHeap ready_at_t = create_taskheap(tg.n);

    int t = 0, i = 0, s = st.top, n_scheduled = 0; // number of scheduled tasks
    int *schedule = (int *)malloc(tg.n * sizeof(int));
    for (int i = 0; i < tg.n; i++)
        schedule[i] = -1;

    while (n_scheduled < tg.n) {

        // updates the heap with respect to the new value of t
        while (i < tg.n && tg.tasks[i].release_time <= t) {
            if (schedule[i] == -1)
                add_th(&ready_at_t, tg.tasks[i]);
            i++;
        }

        // step 1
        if (is_empty_th(&ready_at_t)) {
            while (i < tg.n && schedule[i] != -1)
                i++;
            // i is the index of the first unscheduled task in the array ordered
            // by growing release time, or is tg.n
            if (i < tg.n) {
                t = tg.tasks[i].release_time;
                // updates the tasks ready at t with respect to the new value of
                // t
                while (i < tg.n && tg.tasks[i].release_time <= t) {
                    if (schedule[i] == -1)
                        add_th(&ready_at_t, tg.tasks[i]);
                    i++;
                }
            }
        }

        // step 2
        while (s >= 0 && st.items[s].end < t) { // updates s
            s--;
        }
        if (s >= 0 && st.items[s].start < t &&
            t < st.items[s].end) { // if t is in a forbidden zone
            t = st.items[s].end;   // update t
            // updates the tasks ready at t with respect to the new value of
            // t
            while (i < tg.n && tg.tasks[i].release_time <= t) {
                if (schedule[i] == -1)
                    add_th(&ready_at_t, tg.tasks[i]);
                i++;
            }
            if (s < st.top)
                s++;
        }

        // step 3
        Task to_sched = pop_th(&ready_at_t);
        schedule[to_sched.i] = t;
        n_scheduled++;
        t += D;
    }

    free_taskheap(&ready_at_t);
    return schedule;
}

int *schedule_greedy(Taskgroup tg)
{
    // try to greedily schedule the tasks and returns NULL if it failed. We
    // suppose that the tasks are sorted by growing release time
    TaskHeap ready_at_t = create_taskheap(tg.n);

    int t = 0, i = 0, n_scheduled = 0; // number of scheduled tasks
    int *schedule = (int *)malloc(tg.n * sizeof(int));
    for (int i = 0; i < tg.n; i++)
        schedule[i] = -1;

    while (n_scheduled < tg.n) {

        // updates the heap with respect to the new value of t
        while (i < tg.n && tg.tasks[i].release_time <= t) {
            if (schedule[i] == -1)
                add_th(&ready_at_t, tg.tasks[i]);
            i++;
        }

        if (is_empty_th(&ready_at_t)) {
            while (i < tg.n && schedule[i] != -1)
                i++;
            // i is the index of the first unscheduled task in the array ordered
            // by growing release time, or is tg.n
            if (i < tg.n) {
                t = tg.tasks[i].release_time;
                // updates the tasks ready at t with respect to the new value of
                // t
                while (i < tg.n && tg.tasks[i].release_time <= t) {
                    if (schedule[i] == -1)
                        add_th(&ready_at_t, tg.tasks[i]);
                    i++;
                }
            }
        }

        Task to_sched = pop_th(&ready_at_t);
        if (to_sched.deadline < t + D) { // if the deadline is not respected
            free(schedule);
            return NULL;
        }
        schedule[to_sched.i] = t;
        n_scheduled++;
        t += D;
    }

    free_taskheap(&ready_at_t);
    return schedule;
}

bool is_valid(Taskgroup tg, int *schedule)
{
    // checks if a given schedule is valid
    for (int i = 0; i < tg.n; i++)
        if (schedule[i] < tg.tasks[i].release_time ||
            schedule[i] + D > tg.tasks[i].deadline)
            return false;
    return true;
}