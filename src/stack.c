#include <stdio.h>
#include <stdlib.h>

#include "stack.h"

int max(int a, int b) { return (a >= b) ? a : b; }

int min(int a, int b) { return (a <= b) ? a : b; }

Stack create_stack(int n)
{
    Interval *intervals = (Interval *)malloc(n * sizeof(Interval));
    return (Stack){.maxsize = n, .top = -1, .items = intervals};
}

bool is_empty_st(Stack *st) { return st->top == -1; }

bool is_full_st(Stack *st) { return st->top + 1 == st->maxsize; }

Interval pop_st(Stack *st)
{
    if (is_empty_st(st)) {
        fprintf(stderr, "Error : the stack is empty\n");
        exit(EXIT_FAILURE);
    }

    st->top = st->top - 1;
    return st->items[st->top + 1];
}

void push_st(Stack *st, Interval i)
{
    if (is_full_st(st)) {
        fprintf(stderr, "Error : the stack is full\n");
        exit(EXIT_FAILURE);
    }

    st->top = st->top + 1;
    st->items[st->top] = i;
}

// merges the last 2 intervals in the stack if they exist and they overlap
// We know the beginning of the last element is lower than the beginning of the
// second to last element
void merge_last_st(Stack *st)
{
    if (st->top <= 0) // there isn't 2 elements in the stack
        return;

    Interval last = st->items[st->top], last_last = st->items[st->top - 1];
    if (last.end > last_last.start) {
        st->items[st->top - 1].start =
            min(last.start, last_last.start); // merge
        st->items[st->top - 1].end = max(last.end, last_last.end);
        st->top = st->top - 1;
    }
}

void show_stack(Stack *st)
{
    if (is_empty_st(st))
        printf("Empty stack !\n");

    int i = 0;
    while (i <= st->top) {
        printf("Interval ]%d; %d[\n", st->items[i].start, st->items[i].end);
        i++;
    }
    printf("---\n");
}

void free_stack(Stack *st) { free(st->items); }

/*int main()
{
    Stack st = create_stack(50);
    Interval i1 = (Interval) {.start = 1, .end = 3};
    Interval i2 = (Interval) {.start = 0, .end = 2};
    push(&st, i1);
    push(&st, i2);
    show_stack(&st);
    merge_last(&st);
    show_stack(&st);

    pop(&st);
    show_stack(&st);

    free_stack(&st);
}*/