#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "const.h"
#include "stack.h"
#include "taskload_tree.h"
#define DEBUG false
#define ntasks 9

// comparison function to sort the tasks by growing deadline
int cmp_func_deadline(const void *a, const void *b)
{
    Element T1 = *(Element *)a;
    Element T2 = *(Element *)b;
    return T1.deadline - T2.deadline;
}

int cmp_func_tasks_deadline(const void *a, const void *b) // TODO remove
{
    Task T1 = *(Task *)a;
    Task T2 = *(Task *)b;
    return T1.deadline - T2.deadline;
}

bool is_full_tlt(TaskLoadTree *tlt) { return tlt->maxsize == tlt->last + 1; }

bool is_empty_tlt(TaskLoadTree *tlt) { return tlt->last == -1; }

void show_tlt(TaskLoadTree *tlt)
{
    if (is_empty_tlt(tlt)) {
        printf("The tree is empty\n");
        return;
    }
    for (int i = 0; i <= tlt->last; i++) {
        printf("Elt n°%d of deadline %d, n_part %d, indexes l%d r%d\n", i,
               tlt->items[i].deadline, tlt->items[i].n_part,
               tlt->items[i].i_left, tlt->items[i].i_right);
    }
    printf("\n----\n");
}

int get_taskload(TaskLoadTree *tlt, int i, int deadline)
// returns the taskload corresponding to the given deadline. i is the index of
// the current node taskload is obtained by summing the n_part from the root to
// the corresponding deadline
{
    Element node = tlt->items[i];

    if (node.deadline == deadline) {
        return node.n_part;
    }
    else if (node.deadline > deadline) { // search on the left
        if (node.i_left == -1) {
            fprintf(stderr,
                    "Error : the deadline %d doesn't exist in the tree\n",
                    deadline);
            exit(EXIT_FAILURE);
        }
        return node.n_part + get_taskload(tlt, node.i_left, deadline);
    }
    else { // search on the right
        if (node.i_right == -1) {
            fprintf(stderr,
                    "Error : the deadline %d doesn't exist in the tree\n",
                    deadline);
            exit(EXIT_FAILURE);
        }
        return node.n_part + get_taskload(tlt, node.i_right, deadline);
    }
}

void increment_taskload(TaskLoadTree *tlt, int i, int deadline)
// increments the taskload corresponding to the given deadline. i is the index
// of the current node taskload is obtained by summing the n_part from the root
// to the corresponding deadline
{
    Element node = tlt->items[i];
    int i_left = node.i_left;

    if (node.deadline == deadline) {
        tlt->items[i].n_part += 1;
        if (i_left != -1)
            tlt->items[i_left].n_part -= 1;
    }
    else if (node.deadline > deadline) { // search on the left
        if (node.i_left == -1) {
            fprintf(stderr,
                    "Error : the deadline %d doesn't exist in the tree\n",
                    deadline);
            exit(EXIT_FAILURE);
        }
        tlt->items[i].n_part += 1;
        if (i_left != -1) // decrement left son bc it's lower
            tlt->items[i_left].n_part -= 1;

        increment_taskload(tlt, i_left, deadline);
    }
    else { // search on the right
        if (node.i_right == -1) {
            fprintf(stderr,
                    "Error : the deadline %d doesn't exist in the tree\n",
                    deadline);
            exit(EXIT_FAILURE);
        }
        increment_taskload(tlt, node.i_right, deadline);
    }
}

TaskLoadTree create_empty_tlt(int n)
{
    Element *items = (Element *)malloc(n * sizeof(Element));
    return (TaskLoadTree){.items = items, .maxsize = n, .last = -1};
}

void free_tlt(TaskLoadTree *tlt) { free(tlt->items); }

void fill_tlt_rec(TaskLoadTree *tlt, int left, int right, int i)
{
    int i_left = max(left, (left + i - 1) / 2),
        i_right = min(right, (right + i + 1) / 2);

    if (i_left != i)
        tlt->items[i].i_left = i_left;

    if (i_right != i)
        tlt->items[i].i_right = i_right;

    if (left <= i - 1)
        fill_tlt_rec(tlt, left, i - 1, i_left);

    if (right >= i + 1)
        fill_tlt_rec(tlt, i + 1, right, i_right);
}

void fill_tlt(TaskLoadTree *tlt, Taskgroup *tg)
{
    if (!is_empty_tlt(tlt)) {
        fprintf(stderr, "The taskload tree must be empty to fill it\n");
        exit(EXIT_FAILURE);
    }
    if (tg->n > tlt->maxsize) {
        fprintf(stderr, "The taskload tree is too small\n");
        exit(EXIT_FAILURE);
    }
    // copies the tasks in the taskload tree and sorts them by growing deadline

    for (int i = 0; i < tg->n; i++)
        tlt->items[i] = (Element){.deadline = tg->tasks[i].deadline,
                                  .n_part = 0,
                                  .i_left = -1,
                                  .i_right = -1};
    qsort(tlt->items, tg->n, sizeof(Element), cmp_func_deadline);
    tlt->last = tg->n - 1; // index of last element

    int left = 0, right = tlt->last, m = (right - left) / 2;
    fill_tlt_rec(tlt, left, right, m);
}

/*

void test1()
{
    Task test[ntasks];
    test[0].deadline = 7;
    test[1].deadline = 13;
    test[2].deadline = 5;
    test[3].deadline = 10;
    test[4].deadline = 13;
    test[5].deadline = 8;
    test[6].deadline = 5;
    test[7].deadline = 4;
    test[8].deadline = 14;

    TaskLoadTree tlt = create_empty_tlt(ntasks);
    Taskgroup tg = (Taskgroup){.n = ntasks, .tasks = test};
    fill_tlt(&tlt, &tg);
    show_tlt(&tlt);

    increment_taskload(&tlt, tlt.last / 2, 13);
    increment_taskload(&tlt, tlt.last / 2, 8);
    increment_taskload(&tlt, tlt.last / 2, 5);
    increment_taskload(&tlt, tlt.last / 2, 7);
    show_tlt(&tlt);
    int d = 4;
    printf("Taskload of deadline %d : %d\n", d,
           get_taskload(&tlt, tlt.last / 2, d));
    d = 13;
    printf("Taskload of deadline %d : %d\n", d,
           get_taskload(&tlt, tlt.last / 2, d));
    d = 10;
    printf("Taskload of deadline %d : %d\n", d,
           get_taskload(&tlt, tlt.last / 2, d));
}

void test_random()
{
    // calculates randomly generated taskloads with both a naive approach and a
    // taskload tree. Checks that both return the same thing when request

    int n = 1000, n_modif = 100, n_req = 1000; // number of tasks, number of
                                        // modifications and number of requests
    int dmax = 10 * n;
    Task *tasks = (Task *)malloc(n * sizeof(Task));
    int *taskloads = (int *)calloc(n, sizeof(int));

    // generates random tasks
    for (int i = 0; i < n; i++)
        tasks[i].deadline = rand() % dmax;
    Taskgroup tg = (Taskgroup){.n = n, .tasks = tasks};
    TaskLoadTree tlt = create_empty_tlt(n);
    fill_tlt(&tlt, &tg);
    qsort(tasks, n, sizeof(Task), cmp_func_tasks_deadline);

    for (int i = 0; i < n_modif; i++) {
        int i_d = rand() % n, d = tasks[i_d].deadline;
        if (DEBUG)
            printf("Incrementing npart for deadline %d\n", d);
        // we want to increment the taskloads of all deadlines >= d
        increment_taskload(&tlt, tlt.last / 2,
                           d); // increments the taskload in the tree

        while (i_d > 0 && tasks[i_d].deadline == tasks[i_d - 1].deadline)
            i_d--;
        for (int j = i_d; j < n; j++) // increments the taskloads in the array
            taskloads[j] += 1;
    }
    if (DEBUG) {
        show_tlt(&tlt);
        printf("Taskloads : ");
        for (int i = 0; i < n; i++)
            printf(" | d : %d, tl : %d", tasks[i].deadline, taskloads[i]);
        printf("\n");
    }

    for (int i = 0; i < n_req; i++) {
        int i_d = rand() % n, d = tasks[i_d].deadline;
        if (DEBUG)
            printf("Requesting taskload for deadline %d. Naive returns %d and "
                   "tree "
                   "returns %d\n",
                   d, taskloads[i_d], get_taskload(&tlt, tlt.last / 2, d));
        // we want to make sure that the tree and the array returns the same
        // thing
        assert(taskloads[i_d] == get_taskload(&tlt, tlt.last / 2, d));
    }

    free(tasks);
    free(taskloads);
}

int main()
{
    srand((unsigned)time(NULL)); // initializes the seed
    for (int i = 0; i < 1000; i++)
        test_random();
    printf("The algorithm is correct\n");

    return 0;
}*/