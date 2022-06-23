#ifndef HEAP_SCHED
#define HEAP_SCHED

#include "scheduling.h"

struct task_wid { // a task accompanied by its index in the original task array
    Task t;
    int i;
};
typedef struct task_wid Task_wid;

// a heap of tasks ordered by min deadline
struct TaskHeap {
    int maxsize;
    int last;        // index of last element (0 initially)
    Task_wid *items; // first case of items is empty
};
typedef struct TaskHeap TaskHeap;

TaskHeap create_taskheap(int n);
bool is_empty_th(TaskHeap *th);
bool is_full_th(TaskHeap *th);
void free_taskheap(TaskHeap *th);
void exchange(TaskHeap *th, int i, int j);
void add_th(TaskHeap *th, Task t, int i);
void equilibrate_parent(TaskHeap *th, int i);
Task_wid pop_th(TaskHeap *th);
bool is_full_th(TaskHeap *th);
void show_taskheap(TaskHeap *th);

#endif