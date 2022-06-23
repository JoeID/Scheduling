#include <assert.h> // TODO remove
#include <stdio.h>
#include <stdlib.h>
#include <time.h> // TODO remove

#include "const.h"
#include "heap.h"
#include "scheduling.h"

TaskHeap create_taskheap(int n)
{
    Task_wid *items = (Task_wid *)malloc((n + 1) * sizeof(Task_wid));
    return (TaskHeap){.items = items, .maxsize = n, .last = 0};
}

void free_taskheap(TaskHeap *th) { free(th->items); }

void exchange(TaskHeap *th, int i, int j)
{
    Task_wid tmp = th->items[i];
    th->items[i] = th->items[j];
    th->items[j] = tmp;
}

bool is_full_th(TaskHeap *th) { return th->last == th->maxsize; }

bool is_empty_th(TaskHeap *th) { return th->last == 0; }

void equilibrate_parent(TaskHeap *th, int i)
{
    // exchanges th->items[i] with its parents until its deadline is greater
    // than its parent'
    while (i > 1 && th->items[i].t.deadline < th->items[i / 2].t.deadline) {
        exchange(th, i, i / 2);
        i = i / 2;
    }
}

void equilibrate_child(TaskHeap *th, int i)
{
    // exchanges th->items[i] with the min of its children until the heap
    // structure is respected
    while (true) {
        if (2 * i > th->last) // no more children
            return;

        if (2 * i + 1 > th->last) { // no right child

            int d_left = th->items[2 * i].t.deadline;
            int d = th->items[i].t.deadline;
            if (d > d_left)
                exchange(th, i, 2 * i);
            return;
        }
        else {

            int d_left = th->items[2 * i].t.deadline;
            int d_right = th->items[2 * i + 1].t.deadline;
            int d = th->items[i].t.deadline;

            if (d > d_left && d_left <= d_right) {
                // exchange with left child
                exchange(th, i, 2 * i);
                i = 2 * i;
                continue;
            }
            if (d > d_right && d_right <= d_left) {
                // exchange with right child
                exchange(th, i, 2 * i + 1);
                i = 2 * i + 1;
                continue;
            }
            return;
        }
    }
}

void add_th(TaskHeap *th, Task t, int i)
{
    if (is_full_th(th)) {
        fprintf(stderr, "Error : the heap is full\n");
        exit(EXIT_FAILURE);
    }
    th->last = th->last + 1;
    th->items[th->last] = (Task_wid){.i = i, .t = t};
    equilibrate_parent(th, th->last);
}

Task_wid pop_th(TaskHeap *th)
{
    if (is_empty_th(th)) {
        fprintf(stderr, "Error : the heap is empty\n");
        exit(EXIT_FAILURE);
    }

    Task_wid res = th->items[1];
    th->items[1] = th->items[th->last];
    th->last = th->last - 1;
    equilibrate_child(th, 1);
    return res;
}

void show_taskheap(TaskHeap *th)
{
    if (is_empty_th(th)) {
        printf("The heap is empty\n");
        return;
    }

    int to_print = 1;
    for (int i = 1; i <= th->last; i++) {
        printf("Task %d %d ", th->items[i].t.release_time,
               th->items[i].t.deadline);
        to_print--;
        if (to_print == 0) {
            to_print = i + 1;
            printf("\n");
        }
    }
    printf("\n----\n");
}
/*
int main()
{
    int n = 10;
    TaskHeap th = create_taskheap(n);
    srand((unsigned)time(NULL));

    Task T1 = (Task) {.release_time = 0, .deadline = 0};
    Task T2 = (Task) {.release_time = 0, .deadline = 1};
    Task T3 = (Task) {.release_time = 0, .deadline = 2};
    add_th(&th, T1);
    add_th(&th, T2);
    add_th(&th, T3);
    show_taskheap(&th);
    pop_th(&th);
    show_taskheap(&th);
    pop_th(&th);
    show_taskheap(&th);
    pop_th(&th);
    show_taskheap(&th);


    //show_taskheap(&th);
    for(int j = 0; j < 1000000; j++) {
        for (int i = 0; i < n; i++) {
            add_th(&th, (Task){.release_time = 0, .deadline = rand() % n});
        }
        int old_d = 0;
        for (int i = 0; i < n; i++) {
            Task t = pop_th(&th);
            assert(old_d <= t.deadline);
            old_d = t.deadline;
            //printf("Task %d %d \n", t.release_time, t.deadline);
            //printf("%d is extracted\n", t.deadline);
            //show_taskheap(&th);
        }
        //show_taskheap(&th);
    }
    printf("The algorithm is correct\n");
    free_taskheap(&th);
    return 0;
}*/