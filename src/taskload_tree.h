#ifndef TASKLOAD_SCHED
#define TASKLOAD_SCHED

#include "const.h"
#include "scheduling.h"

struct Element {
    int deadline; // the tree is ordered by deadline
    int n_part;
    int i_left;   // index of left child
    int i_right;  // index of right child
};
typedef struct Element Element;

struct TaskLoadTree {
    int maxsize;
    int last;
    Element *items;
};
typedef struct TaskLoadTree TaskLoadTree;

int cmp_func_deadline(const void *a, const void *b);
bool is_full_tlt(TaskLoadTree *tlt);
bool is_empty_tlt(TaskLoadTree *tlt);
void show_tlt(TaskLoadTree *tlt);
int get_taskload(TaskLoadTree *tlt, int i, int deadline);
void increment_taskload(TaskLoadTree *tlt, int i, int deadline);
TaskLoadTree create_empty_tlt(int n);
void free_tlt(TaskLoadTree *tlt);
void fill_tlt_rec(TaskLoadTree *tlt, int left, int right, int i);
void fill_tlt(TaskLoadTree *tlt, Taskgroup *tg);

#endif