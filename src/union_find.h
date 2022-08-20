#include "taskload_tree.h"

#ifndef UF_TREES_SCHED
#define UF_TREES_SCHED

/* A UNION-FIND DATA STRUCTURE TO MANIPULATE THE PSEUDO OFFSETS*/

// implements the ranks but not compression path (useless for our algo)
struct node {
    struct node *parent;
    int rank;
    int o_part;
    int deadline;
};
typedef struct node Node;

// we put the nodes in a contiguous area of memory to limit the number of calls
// to malloc()
typedef struct {
    Node *nodes;
    int size; // size is the number of nodes, not the maximum capacity
} Forest;

Forest create_empty_f(int n);
void fill_forest(Forest *f, Taskgroup *tg);
void make_set(Node *x);
Node *find(Node *x);
void union_f(Node *x, Node *y);

void free_f(Forest *f);

#endif