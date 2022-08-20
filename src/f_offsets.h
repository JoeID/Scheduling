#include "const.h"
#include "scheduling.h"
#include "taskload_tree.h"
#include "union_find.h"

#ifndef FRACT_OFFSET_TREE_SCHED
#define FRACT_OFFSET_TREE_SCHED

struct pair {
    int x;
    int y;
};
typedef struct pair Pair;

struct offset_elt {
    int f_offset;
    Node *tree;  // the uf tree corresponding to the f_offset
    int i_left;  // BST structure : < on the left, > on the right
    int i_right; // all the nodes have a different value of f_offset
    int i_parent;
};
typedef struct offset_elt Offset_elt;

// the nodes are stored in an array to minimize the number of mallocs
struct fOffsets {
    Offset_elt *items;
    int i_root; // index of root
    int size;   // actual size
    Forest forest; // corresponding union-find forest 
};
typedef struct fOffsets FOffsets;

struct vector_int { // array with its size
    int *vect;
    int size;
};
typedef struct vector_int Vector_int;

int cmp_func_offset(const void *a, const void *b);
int cmp_func_int(const void *a, const void *b);
bool is_in_fot(FOffsets *fot, int i, int f_offset);
bool is_empty_fot(FOffsets *fot);
void show_fot(FOffsets *fot);
FOffsets create_empty_fot(int n);
void free_fot(FOffsets *fot);

void fill_fot_rec(FOffsets *fot, int left, int right, int i);
void fill_fot(FOffsets *fot, Taskgroup *tg);
int leftmost(FOffsets *fot, int i);
int rightmost(FOffsets *fot, int i);
void replace_node(FOffsets *fot, int i, int old, int new);
void interval(FOffsets *fot, int a, int b, Vector_int *v, int i);
void remove_node(FOffsets *fot, int i, int f_offset);
void replace_fot(FOffsets *fot, int a, int b, Vector_int *vect);

#endif