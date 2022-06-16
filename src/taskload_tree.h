#ifndef TASKLOAD_SCHED
#define TASKLOAD_SCHED

#include "const.h"
#include "scheduling.h"

/*
A taskload tree works as follows : it's a BST ordered by deadline (lower on the
left), and each node contains an auxiliary integer such as the taskload
corresponding to that deadline is the sum of all auxiliary integers from the
root to the node if the path turns right

It is useful because if we have a deadline di and we want to add 1 to all
deadlines dj >= di, it can be done in O(logn) by simply incrementing the
auxiliary integer of di, provided that the tree is balanced
*/

struct Element {
    int deadline; // the tree is ordered by deadline
    int n_part;
    int i_left;  // index of left child
    int i_right; // index of right child
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