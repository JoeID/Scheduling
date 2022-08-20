#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "test_gen.h"
#include "union_find.h"

Forest create_empty_f(int n)
{
    Node *nodes = (Node *)malloc(n * sizeof(Node));
    return (Forest){
        .size = -1,
        .nodes =
            nodes}; // size is the number of nodes, not the maximum capacity
}

void fill_forest(Forest *f, Taskgroup *tg)
{
    for (int i = 0; i < tg->n; i++)
        f->nodes[i] = (Node){.deadline = tg->tasks[i].deadline % D, .o_part = 0};
    for (int i = 0; i < tg->n; i++)
        make_set(&f->nodes[i]);
    f->size = tg->n;
}

void make_set(Node *x)
{
    x->parent = NULL;
    x->rank = 0;
}

Node *find(Node *x)
{
    if (!x->parent)
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

void show_f(Forest *f)
{
    for (int i = 0; i < f->size; i++) {
        printf("Node %p of parent %p, deadline %d, rank %d\n", &f->nodes[i],
               f->nodes[i].parent, f->nodes[i].deadline, f->nodes[i].rank);
    }
}
/*
void test_random()
{
    // generates random tests to make sure that the union-find functions are
correct int N = 10, n_union = 100; for(int n = 1; n <= 1000; n++){ Taskgroup
*tgs = get_generate_testcases(N, n); for(int i = 0; i < N; i++){ Forest f =
create_empty_f(n); fill_forest(&f, &tgs[i]); Node **naive = (Node**) malloc(n *
sizeof(Node*)); for(int i = 0; i < n; i++) naive[i] = find(&f.nodes[i]);
            // naive[i] : adresse du représentant de f.nodes[i]

            for(int j = 0; j < n_union; j++){
                // merges two random nodes
                int i1 = 0, i2 = 0;
                while(i1 == i2)
                    i1 = rand() % n, i2 = rand() % n;
                union_f(&f.nodes[i1], &f.nodes[i2]);
                for(int k = 0; k < n; k++)
                    if(fin)
            }

            for(int j = 0; j < n; j++)
                assert(find(naive[j]) == find(&f.nodes[j]));
        }
    }
}

Taskgroup get_tasks_bis(FILE *file)
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
Taskgroup *get_taskgroups_bis(FILE *file, int *N)
{
    fscanf(file, "%d\n", N);
    Taskgroup *taskgroups = (Taskgroup *)malloc((*N) * sizeof(Taskgroup));
    for (int i = 0; i < (*N); i++) {
        fscanf(file, "\n");
        taskgroups[i] = get_tasks_bis(file);
        fscanf(file, "\n");
    }
    return taskgroups;
}

void show_tasks_bis(Taskgroup tg)
{
    for (int i = 0; i < tg.n; i++)
        printf("Task %d(%d, %d) ", i, tg.tasks[i].release_time,
               tg.tasks[i].deadline);
    printf("\n");
}

int main()
{
    int n = 10, N;
    printf("Forest test\n");
    Forest f = create_empty_f(n);
    FILE *src = fopen("sched_tests/debug", "r");
    Taskgroup *tgs = get_taskgroups_bis(src, &N);
    fclose(src);
    show_tasks_bis(tgs[0]);
    fill_forest(&f, tgs);
    show_f(&f);
    Node *x = &f.nodes[0], *y = &f.nodes[1];
    union_f(x, y);
    show_f(&f);

    for (int i = 0; i < N; i++)
        free(tgs[i].tasks);
    free(tgs);

    free_f(&f);

    test_random();
    printf("The algorithm is correct\n");
    return 0;
}
*/