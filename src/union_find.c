#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "union_find.h"

Forest create_empty_f(int n)
{
    Node *nodes = (Node *)malloc(n * sizeof(Node));
    return (Forest){.last = -1, .maxsize = n, .nodes = nodes};
}

void fill_forest(Forest *f, Taskgroup *tg)
{
    for (int i = 0; i < tg->n; i++)
        f->nodes[i] = (Node){.deadline = tg->tasks[i].deadline, .o_part = 0};
    for (int i = 0; i < tg->n; i++)
        make_set(&f->nodes[i]);
}

void make_set(Node *x)
{
    x->parent = NULL;
    x->rank = 0;
}

Node *find(Node *x)
{
    if (x->parent == x)
        return x;
    return find(x->parent);
}

void union_f(Node *x, Node *y) // TODO compression path
{
    Node *x_root = find(x);
    Node *y_root = find(y);
    if (x_root != y_root) {
        // attaches the tree of smallest rank to the other
        if (x_root->rank < y_root->rank) {
            x_root->parent = y_root;
        }
        else {
            y_root->parent = y_root;
            if (y_root->rank == x_root->rank)
                x_root->rank = x_root->rank + 1;
        }
    }
}

void free_f(Forest *f) { free(f->nodes); }

/*
int main() { return 0; }
*/