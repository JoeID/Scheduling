#include <stdio.h>
#include <stdlib.h>

#include "const.h"
#include "stack.h"
#include "taskload_tree.h"
#define ntasks 9

int max(int a, int b) { return (a >= b) ? a : b; }

int min(int a, int b) { return (a <= b) ? a : b; }

// comparison function to sort the tasks by growing deadline
int cmp_func_deadline(const void *a, const void *b)
{
    Element T1 = *(Element *)a;
    Element T2 = *(Element *)b;
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
        printf("Elt n°%d of deadline %d, n_part %d, indexes l%d r%d\n",
               i,
               tlt->items[i].deadline,
               tlt->items[i].n_part,
               tlt->items[i].i_left,
               tlt->items[i].i_right);
    }
    printf("\n----\n");
}

int get_taskload(TaskLoadTree *tlt, int i, int deadline)
// returns the taskload corresponding to the given deadline. i is the index of the current node
// taskload is obtained by summing the n_part from the root to the corresponding deadline
{
    Element node = tlt->items[i];

    if(node.deadline == deadline){
        return node.n_part;

    } else if (node.deadline > deadline){ // search on the left

        if(node.i_left == -1){
            fprintf(stderr, "Error : the deadline %d doesn't exist in the tree\n", deadline);
            exit(EXIT_FAILURE);
        }
        return node.n_part + get_taskload(tlt, node.i_left, deadline); 
    } else { // search on the right

        if(node.i_right == -1){
            fprintf(stderr, "Error : the deadline %d doesn't exist in the tree\n", deadline);
            exit(EXIT_FAILURE);
        }
        return node.n_part + get_taskload(tlt, node.i_right, deadline); 
    }
}

void increment_taskload(TaskLoadTree *tlt, int i, int deadline) 
// increments the taskload corresponding to the given deadline. i is the index of the current node
// taskload is obtained by summing the n_part from the root to the corresponding deadline
{
    Element node = tlt->items[i];
    /*printf("Elt n°%d of deadline %d, n_part %d, indexes l%d r%d\n",
            i,
            node.deadline,
            node.n_part,
            node.i_left,
            node.i_right);*/

    if(node.deadline == deadline){

        tlt->items[i].n_part += 1;
    } else if (node.deadline > deadline){ // search on the left

        if(node.i_left == -1){
            fprintf(stderr, "Error : the deadline %d doesn't exist in the tree\n", deadline);
            exit(EXIT_FAILURE);
        }
        increment_taskload(tlt, node.i_left, deadline);
    } else { // search on the right

        if(node.i_right == -1){
            fprintf(stderr, "Error : the deadline %d doesn't exist in the tree\n", deadline);
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

int main()
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
    printf("Taskload of deadline %d : %d\n", d, get_taskload(&tlt, tlt.last / 2, d));
    return 0;
}