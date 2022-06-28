#include "union_find.h"

#ifndef FRACT_OFFSET_TREE_SCHED
#define FRACT_OFFSET_TREE_SCHED

struct f_offset {
    int f_offset;
    Node *f_tree;
};
typedef struct f_offset *int;

#endif