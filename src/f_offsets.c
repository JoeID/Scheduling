#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "const.h"
#include "f_offsets.h"
#include "scheduling.h"
#include "union_find.h"
#define DEBUG false

/*
Data structures that allows to create a BST, then perform modifs
like :
"given a and b, replace all nodes that have keys in ]a,b[ (if a < b) or ]0,
b[U]a, D[ (else) by a single node of key a, and keep the BST structure"
²*/

void show_arr(int *arr, int n)
{
    for (int i = 0; i < n; i++)
        printf("%d ", arr[i]);
    printf("\n");
}

int cmp_func_offset(const void *a, const void *b)
{
    Offset_elt T1 = *(Offset_elt *)a;
    Offset_elt T2 = *(Offset_elt *)b;
    return T1.f_offset - T2.f_offset;
}

int cmp_func_int(const void *a, const void *b)
{
    return *((int *)a) - *((int *)b);
}

bool is_in_fot(FOffsets *fot, int i, int f_offset)
// true if f_offset is in the tree
{
    if (i == -1)
        return false;

    if (f_offset == fot->items[i].f_offset) {
        return true;
    }
    else if (f_offset < fot->items[i].f_offset) {
        return is_in_fot(fot, fot->items[i].i_left, f_offset);
    }
    else {
        return is_in_fot(fot, fot->items[i].i_right, f_offset);
    }
}

bool is_empty_fot(FOffsets *fot) { return fot->size == 0; }

void show_fot(FOffsets *fot)
{
    if (is_empty_fot(fot)) {
        printf("The f_offset array is empty\n");
        return;
    }
    printf("Root at %d\n", fot->i_root);
    for (int i = 0; i < fot->size; i++) {
        printf("%d l%d r%d p%d\n", fot->items[i].f_offset, fot->items[i].i_left,
               fot->items[i].i_right, fot->items[i].i_parent);
    }
    printf("\n----\n");
}

FOffsets create_empty_fot(int n)
{
    Offset_elt *items = (Offset_elt *)malloc(n * sizeof(Offset_elt));
    Forest forest = create_empty_f(n);
    return (FOffsets){.items = items, .size = 0, .forest = forest};
}

void free_fot(FOffsets *fot) { 
    free(fot->items); 
    free_f(&fot->forest);
}

void fill_fot_rec(FOffsets *fot, int left, int right, int i)
{
    int i_left = max(left, (left + i - 1) / 2),
        i_right = min(right, (right + i + 1) / 2);

    if (i_left != i) {
        fot->items[i].i_left = i_left;
        fot->items[i_left].i_parent = i;
    }

    if (i_right != i) {
        fot->items[i].i_right = i_right;
        fot->items[i_right].i_parent = i;
    }

    if (left <= i - 1)
        fill_fot_rec(fot, left, i - 1, i_left);

    if (right >= i + 1)
        fill_fot_rec(fot, i + 1, right, i_right);
}

void fill_fot(FOffsets *fot, Taskgroup *tg)
// fills the array of f_offsets with the tasks and sorts them by growing
// f_offset. Supposes there is enough place
{
    if (!is_empty_fot(fot)) {
        fprintf(stderr,
                "Error in %s : the f_offset array must be empty to fill it\n",
                __func__);
        exit(EXIT_FAILURE);
    }
    // copies the tasks in the taskload tree and sorts them by growing deadline

    fill_forest(&fot->forest, tg);

    for (int i = 0; i < tg->n; i++)
        fot->items[i] = (Offset_elt){.f_offset = tg->tasks[i].deadline % D,
                                     .i_left = -1,
                                     .i_right = -1,
                                     .i_parent = -1,
                                     .tree = &fot->forest.nodes[i]};
    fot->size = tg->n;
    qsort(fot->items, fot->size, sizeof(Offset_elt), cmp_func_offset);
    // then we have to remove duplicates
    int i, j = 1;
    for (i = 1; i < tg->n; i++){
        if (fot->items[i - 1].f_offset != fot->items[i].f_offset){
            fot->items[j++].f_offset = fot->items[i].f_offset;
        } else { // merge trees according to same f_offset
            union_f(fot->items[i - 1].tree, fot->items[i].tree);
        }
    }
    fot->size = j;
    fot->i_root = (j - 1) / 2;

    int left = 0, right = fot->size - 1, m = (right - left) / 2;
    fill_fot_rec(fot, left, right, m);
}

int leftmost(FOffsets *fot, int i)
// returns the index of the leftmost node of the tree of root i
{
    return (fot->items[i].i_left == -1) ? i
                                        : leftmost(fot, fot->items[i].i_left);
}

int rightmost(FOffsets *fot, int i)
// returns the index of the rightmost node of the tree of root i
{
    return (fot->items[i].i_right == -1)
               ? i
               : rightmost(fot, fot->items[i].i_right);
}

void remove_node(FOffsets *fot, int i, int f_offset)
// removes the node corresponding to f_offset
{
    if (i == -1) {
        fprintf(stderr, "Error in %s : the tree doesn't contain a node %d\n",
                __func__, f_offset);
        exit(EXIT_FAILURE);
    }
    int i_left = fot->items[i].i_left, i_right = fot->items[i].i_right,
        i_parent = fot->items[i].i_parent;
    if (fot->items[i].f_offset == f_offset) {

        if (i_left == -1 && i_right == -1) { // i is a leaf
            // remove the node i
            if (fot->items[i_parent].i_left == i) { // if i is left child
                fot->items[i_parent].i_left = -1;
            }
            else { // i is right child
                fot->items[i_parent].i_right = -1;
            }

            if (i == fot->i_root) {
                fprintf(stderr,
                        "Error in %s : can't suppress the tree entirely\n",
                        __func__);
                exit(EXIT_FAILURE);
            }
        }
        else if (i_left == -1) { // i has only right child
            fot->items[i_right].i_parent = i_parent;
            if (i_parent != -1 &&
                fot->items[i_parent].i_left == i) { // if i is left child
                fot->items[i_parent].i_left = i_right;
            }
            else if (i_parent != -1 &&
                     fot->items[i_parent].i_right == i) { // i is right child
                fot->items[i_parent].i_right = i_right;
            }

            if (i == fot->i_root)
                fot->i_root = i_right;
        }
        else if (i_right == -1) { // i has only left child
            fot->items[i_left].i_parent = i_parent;
            if (i_parent != -1 &&
                fot->items[i_parent].i_left == i) { // if i is left child
                fot->items[i_parent].i_left = i_left;
            }
            else if (i_parent != -1 &&
                     fot->items[i_parent].i_right == i) { // i is right child
                fot->items[i_parent].i_right = i_left;
            }

            if (i == fot->i_root)
                fot->i_root = i_left;
        }
        else { // i has two children
            // puts the leftmost child of the right tree instead of i
            int i_swap = leftmost(fot, i_right);
            fot->items[i].f_offset = fot->items[i_swap].f_offset;
            fot->items[i].tree = fot->items[i_swap].tree;

            // i_swap has only one child, suppress the node
            remove_node(fot, i_swap, fot->items[i_swap].f_offset);
        }
    }
    else if (fot->items[i].f_offset > f_offset) { // search on the left
        remove_node(fot, i_left, f_offset);
    }
    else if (fot->items[i].f_offset < f_offset) { // search on the right
        remove_node(fot, i_right, f_offset);
    }
}

void replace_node(FOffsets *fot, int i, int old, int new)
// replaces the unique node of f_offset equal to old to new
{
    if (i == -1) {
        fprintf(stderr, "Error in %s : the node %d is not in the tree\n",
                __func__, old);
        exit(EXIT_FAILURE);
    }
    if (fot->items[i].f_offset == old) {
        fot->items[i].f_offset = new;
        return;
    }
    else if (fot->items[i].f_offset > old) {
        replace_node(fot, fot->items[i].i_left, old, new);
    }
    else if (fot->items[i].f_offset < old) {
        replace_node(fot, fot->items[i].i_right, old, new);
    }
}

void replace_max(FOffsets *fot, int i, int old, int new)
// replaces the node of f_offset equal to old to new but
// suppose that new is greater than any of the other nodes.
// Hence its position has to be changed
{
    if (i == -1) {
        fprintf(stderr, "Error in %s : the node %d is not in the tree\n",
                __func__, old);
        exit(EXIT_FAILURE);
    }
    int i_right = fot->items[i].i_right, i_left = fot->items[i].i_left;

    if (fot->items[i].f_offset == old) {
        assert(i_left == -1); // i should be the min of the tree
        fot->items[i].f_offset = new;

        // remove the link btw i and its parent
        int i_parent = fot->items[i].i_parent;
        if (i_parent != -1 && fot->items[i_parent].i_left == i) {
            // i is left child
            fot->items[i_parent].i_left =
                i_right; // bc i has possibly a right child
            if (i_right != -1)
                fot->items[i_right].i_parent = i_parent;
        }
        else if (i_parent != -1 && fot->items[i_parent].i_right == i) {
            // i is right child
            fot->items[i_parent].i_right =
                i_right; // bc i has possibly a right child
            if (i_right != -1)
                fot->items[i_right].i_parent = i_parent;
        }
        else if (i_parent == -1) {
            // i is the root

            if (i_right != -1) {
                fot->items[i_right].i_parent = -1; // i_right is the new root
                fot->i_root = i_right;
            }
            else {
                // i is the only node
                return;
            }
        }

        // put i as the rightmost node
        int i_max = rightmost(fot, fot->i_root); // index of max
        fot->items[i_max].i_right = i;
        fot->items[i].i_parent = i_max;
        fot->items[i].i_right = -1; // i is max
    }
    else if (fot->items[i].f_offset > old) {
        replace_max(fot, fot->items[i].i_left, old, new);
    }
    else {
        replace_max(fot, fot->items[i].i_right, old, new);
    }
}

void interval(FOffsets *fot, int a, int b, Vector_int *v, int i)
// calculates all the nodes of fot in the interval ]a, b[ and puts them in
// v->vect
{
    if (i == -1)
        return;

    if (fot->items[i].f_offset <= a) {
        interval(fot, a, b, v, fot->items[i].i_right);
    }
    else if (fot->items[i].f_offset >= b) {
        interval(fot, a, b, v, fot->items[i].i_left);
    }
    else {
        v->vect[v->size] = fot->items[i].f_offset;
        v->size = v->size + 1;
        interval(fot, a, b, v, fot->items[i].i_right);
        interval(fot, a, b, v, fot->items[i].i_left);
    }
}

void replace_fot(FOffsets *fot, int a, int b, Vector_int *vect)
// replaces all offsets in ]a,b[ by a if a < b. If a > b, replace offsets in [0,
// b[U]a, D[ by a
{
    vect->size = 0;
    // look for all the f_offsets in ]a,b[
    if (a == b)
        return;

    if (a < b) {
        interval(fot, a, b, vect, fot->i_root);
    }
    else {
        interval(fot, -1, b, vect, fot->i_root);
        interval(fot, a, D, vect, fot->i_root);
    }
    if (DEBUG) {
        printf("Removing :\n");
        show_arr(vect->vect, vect->size);
    }

    if (vect->size == 0) // no node in ]a,b[
        return;

    if (!is_in_fot(fot, fot->i_root, a)) {
        int min_q = __INT32_MAX__, max_q = -1;
        for (int i = 0; i < vect->size; i++)
            min_q = min(min_q, vect->vect[i]),
            max_q = max(max_q, vect->vect[i]);

        if (a < b) {
            for (int i = 0; i < vect->size;
                 i++) // removes them except the lowest
                if (vect->vect[i] != min_q)
                    remove_node(fot, fot->i_root, vect->vect[i]);

            // replaces the lowest with a
            replace_node(fot, fot->i_root, min_q, a);
        }
        else {
            for (int i = 0; i < vect->size;
                 i++) // removes all of them except the max
                if (vect->vect[i] != max_q)
                    remove_node(fot, fot->i_root, vect->vect[i]);

            // a shall be the new max of the tree
            if (max_q > a) {
                // max_q is already the max of the tree
                replace_node(fot, fot->i_root, max_q, a);
            }
            else {
                if (DEBUG) {
                    printf("Calling replace_max on :\n");
                    show_fot(fot);
                }
                // max_q is in [0, b[ so it should be moved in the tree before
                // assigning a
                replace_max(fot, fot->i_root, max_q, a);
            }
        }
    }
    else {
        // removes all nodes in vect
        for (int i = 0; i < vect->size; i++)
            remove_node(fot, fot->i_root, vect->vect[i]);
    }
}

/*
void test1()
{
    int ntasks = 9;
    FOffsets fot = create_empty_fot(ntasks);
    Task test[ntasks];
    int vect[ntasks];
    Vector_int v = (Vector_int){.vect = vect, .size = 0};
    test[0].deadline = 7;
    test[1].deadline = 13;
    test[2].deadline = 5;
    test[3].deadline = 10;
    test[4].deadline = 13;
    test[5].deadline = 8;
    test[6].deadline = 5;
    test[7].deadline = 4;
    test[8].deadline = 14;
    Taskgroup tg = (Taskgroup){.n = ntasks, .tasks = test};
    fill_fot(&fot, &tg);
    show_fot(&fot);
    replace_fot(&fot, 4, 11, &v);
    show_fot(&fot);
    free_fot(&fot);
}

void test2()
{
    int ntasks = 9;
    FOffsets fot = create_empty_fot(ntasks);
    Task test[ntasks];
    int vect[ntasks];
    Vector_int v = (Vector_int){.vect = vect, .size = 0};
    test[0].deadline = 0;
    test[1].deadline = 1;
    test[2].deadline = 1;
    test[3].deadline = 2;
    test[4].deadline = 2;
    test[5].deadline = 4;
    test[6].deadline = 4;
    test[7].deadline = 89;
    test[8].deadline = 90;
    Taskgroup tg = (Taskgroup){.n = ntasks, .tasks = test};
    fill_fot(&fot, &tg);
    show_fot(&fot);
    replace_fot(&fot, -1, 100, &v);
    show_fot(&fot);
    free_fot(&fot);
}

void test_random()
{
    // compares the output of requests of the data structure to that of naive
    // implementation, on random data makes sure that the outputs are the same
    int n = 150, n_modif = 50;
    int *naive = (int *)malloc(n * sizeof(int));
    Task *tasks = (Task *)malloc(n * sizeof(Task));
    FOffsets fot = create_empty_fot(n);
    Vector_int vect = {.size = 0, .vect = (int *)malloc(n * sizeof(int))};

    // fills randomly the arrays

    for (int i = 0; i < n; i++)
        tasks[i].deadline = rand();
    Taskgroup tg = (Taskgroup){.n = n, .tasks = tasks};

    fill_fot(&fot, &tg);
    for (int i = 0; i < fot.size; i++)
        naive[i] = fot.items[i].f_offset;
    if (DEBUG) {
        printf("Tree :\n");
        show_fot(&fot);
    }

    // make n_modif modifications

    for (int i = 0; i < n_modif; i++) {
        int a = rand() % D, b = rand() % D;
        if (DEBUG)
            printf("Modif a, b = %d, %d\n", a, b);
        // modify in the f_offsets structure
        replace_fot(&fot, a, b, &vect);
        // and in the naive array
        for (int j = 0; j < fot.size; j++) {
            if (a < b) {
                if (a < naive[j] && naive[j] < b)
                    naive[j] = a;
            }
            else if (a > b) {
                if ((0 <= naive[j] && naive[j] < b) ||
                    (a < naive[j] && naive[j] < D))
                    naive[j] = a;
            }
        }
        if (DEBUG) {
            printf("Tree after :\n");
            show_fot(&fot);
        }
    }

    // remove duplicates from naive
    qsort(naive, fot.size, sizeof(int), cmp_func_int);
    int n_naive = 1;
    for (int i = 1; i < fot.size; i++)
        if (naive[i - 1] != naive[i])
            naive[n_naive++] = naive[i];
    // extract the values still present in the tree and sort them
    vect.size = 0;
    interval(&fot, -1, D, &vect, fot.i_root);
    qsort(vect.vect, vect.size, sizeof(int), cmp_func_int);

    if (DEBUG) {
        // show_fot(&fot);
        printf("Naive :\n");
        show_arr(naive, n_naive);
        printf("Tree :\n");
        show_arr(vect.vect, vect.size);
    }
    assert(vect.size == n_naive);
    for (int i = 0; i < n_naive; i++)
        assert(vect.vect[i] == naive[i]);

    free_fot(&fot);
    free(tasks);
    free(naive);
    free(vect.vect);
}

int main()
{
    test1();
    test2();
    int N = 100000;
    srand((unsigned)time(NULL)); // initializes the seed
    for (int i = 0; i < N; i++) {
        if (i % (N / 100) == 0)
            printf("%d\n", i / (N / 100));
        test_random();
    }
    printf("\nThe algorithm is correct\n");
    return 0;
}*/