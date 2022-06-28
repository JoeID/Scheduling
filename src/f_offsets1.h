#include "const.h"
#include "scheduling.h"
#include "taskload_tree.h"

#ifndef FRACT_OFFSET_TREE_SCHED
#define FRACT_OFFSET_TREE_SCHED

struct pair {
    int x;
    int y;
};
typedef struct pair Pair;

struct offset_elt {
    int f_offset;
    int *tree;
    int i_left;  // BST structure : < on the left, > on the right
    int i_right; // all the nodes have a different value of f_offset
    int i_parent;
};
typedef struct offset_elt Offset_elt;

// the nodes are stored in an array to minimize the number of mallocs
struct fOffsets {
    Offset_elt *items;
    int maxsize;
    int last; // index of last added element
};
typedef struct fOffsets FOffsets;

int cmp_func_offset(const void *a, const void *b);
bool is_full_fot(FOffsets *fot);
bool is_empty_fot(FOffsets *fot);
void show_fot(FOffsets *fot);
int find_node_inf(FOffsets *fot, int f_offset);

FOffsets create_empty_fot(int n);
void free_fot(FOffsets *fot);
void fill_fot(FOffsets *fot, Taskgroup *tg);
void fill_fot_rec(FOffsets *fot, int left, int right, int i);
void replace_fot(FOffsets *fot, int a, int b);

#endif