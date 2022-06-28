#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "f_offsets1.h"
#include "scheduling.h"
#define DEBUG false

int cmp_func_offset(const void *a, const void *b)
{
    Offset_elt T1 = *(Offset_elt *)a;
    Offset_elt T2 = *(Offset_elt *)b;
    return T1.f_offset - T2.f_offset;
}

bool is_full_fot(FOffsets *fot) { return fot->last == fot->maxsize - 1; }

bool is_empty_fot(FOffsets *fot) { return fot->last == -1; }

void show_fot(FOffsets *fot)
{
    if (is_empty_fot(fot)) {
        printf("The f_offset array is empty\n");
        return;
    }
    for (int i = 0; i <= fot->last; i++) {
        printf("%d ", fot->items[i].f_offset);
    }
    printf("\n----\n");
}

FOffsets create_empty_fot(int n)
{
    Offset_elt *items = (Offset_elt *)malloc(n * sizeof(Offset_elt));
    return (FOffsets){.items = items, .last = -1, .maxsize = n};
}

void free_fot(FOffsets *fot) { free(fot->items); }

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
// f_offset
{
    if (!is_empty_fot(fot)) {
        fprintf(stderr, "The f_offset array must be empty to fill it\n");
        exit(EXIT_FAILURE);
    }
    if (tg->n > fot->maxsize) {
        fprintf(stderr, "The f_offset array is too small\n");
        exit(EXIT_FAILURE);
    }
    // copies the tasks in the taskload tree and sorts them by growing deadline

    for (int i = 0; i < tg->n; i++)
        fot->items[i] =
            (Offset_elt){.f_offset = tg->tasks[i].deadline % D, .tree = NULL};
    qsort(fot->items, tg->n, sizeof(Offset_elt), cmp_func_offset);
    fot->last = tg->n - 1; // index of last element

    int left = 0, right = fot->last, m = (right - left) / 2;
    fill_fot_rec(fot, left, right, m);
}

int find_node(FOffsets *fot, int f_offset, int i)
// returns the index of the occurence of f_offset in the tree, fot->last +
// 1 if it doesn't exist
{
    if (fot->items[i].f_offset == f_offset) {
        return i;
    }
    else if (fot->items[i].f_offset < f_offset && fot->items[i].i_left != -1) {
        return find_node(fot, f_offset, fot->items[i].i_left);
    }
    else if (fot->items[i].f_offset < f_offset && fot->items[i].i_right != -1) {
        return find_node(fot, f_offset, fot->items[i].i_right);
    }
    return i;
}

void remove_node(FOffsets *fot, int i)
// removes the node fot->items[i]
{
    int i_p = fot->items[i].i_parent;
    if (fot->items[i_p].i_left == i) { // if i is left son
        fot->items[i_p].i_left = 0;
    }
}

void replace_fot(FOffsets *fot, int a, int b)
// replaces all offsets in ]a,b[ by a, suppose that a < b
{
    if (a == b || a + 1 == b) // ]a,b[ is empty
        return;
    assert(a < b);
    int i_A = find_node(fot, a, fot->last / 2),
        i_B = find_node(fot, b, fot->last / 2);

    if (i_A > i_B)
        return;

    for (int i = i_A; i <= i_B; i++)
        if (a < fot->items[i].f_offset && fot->items[i].f_offset < b)
            fot->items[i].f_offset = a;
}
/*
void test1()
{
    int ntasks = 9;
    FOffsets fot = create_empty_fot(ntasks);
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
    Taskgroup tg = (Taskgroup){.n = ntasks, .tasks = test};
    fill_fot(&fot, &tg);
    show_fot(&fot);

    free_fot(&fot);
}

void test2()
{
    int ntasks = 9;
    FOffsets fot = create_empty_fot(ntasks);
    Task test[ntasks];
    test[0].deadline = 0;
    test[1].deadline = 2;
    test[2].deadline = 2;
    test[3].deadline = 2;
    test[4].deadline = 2;
    test[5].deadline = 4;
    test[6].deadline = 4;
    test[7].deadline = 4;
    test[8].deadline = 4;
    Taskgroup tg = (Taskgroup){.n = ntasks, .tasks = test};
    fill_fot(&fot, &tg);
    show_fot(&fot);
    replace_fot(&fot, 0, 3);
    show_fot(&fot);
    free_fot(&fot);
}

void show_arr(int *arr, int n)
{
    for(int i = 0; i < n; i++)
        printf("%d ", arr[i]);
    printf("\n");
}

void test_random()
{
    int n = 1000, n_modif = 50;
    int *naive = (int*) malloc(n * sizeof(int));
    Task *tasks = (Task*) malloc(n * sizeof(Task));
    FOffsets fot = create_empty_fot(n);

    // fills randomly the arrays

    for (int i = 0; i < n; i++)
        tasks[i].deadline = rand();
    Taskgroup tg = (Taskgroup){.n = n, .tasks = tasks};

    fill_fot(&fot, &tg);
    for(int i = 0; i < n; i++)
        naive[i] = fot.items[i].f_offset;

    // make n_modif modifications

    for(int i = 0; i < n_modif; i++){
        int a = rand() % D, b = a + rand() % (D - a);
        // modify in the f_offsets structure
        replace_fot(&fot, a, b);
        // and in the naive array
        for(int j = 0; j < n; j++)
            if(a < naive[j] && naive[j] < b)
                naive[j] = a;
    }

    // checks that the arrays are still equal
    for(int i = 0; i < n; i++)
        assert(naive[i] == fot.items[i].f_offset);

    free_fot(&fot);
    free(tasks);
    free(naive);
}

int main()
{
    test1();
    test2();
    int N = 10000;
    srand((unsigned)time(NULL)); // initializes the seed
    for(int i = 0; i < N; i++)
        test_random();
    printf("The algorithm is correct\n");
    return 0;
}*/