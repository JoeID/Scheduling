#ifndef STACK_SCHED
#define STACK_SCHED

#include "const.h"

typedef struct Interval {
    int start;
    int end;
} Interval;

typedef struct Stack {
    int maxsize;
    int top;
    Interval *items;
} Stack;

Stack create_stack(int n);
bool is_empty_st(Stack *st);
bool is_full_st(Stack *st);
Interval pop_st(Stack *st);
void push_st(Stack *st, Interval i);
void merge_last_st(Stack *st);
void show_stack(Stack *st);
bool is_eq_st(Stack *st1, Stack *st2);
void free_stack(Stack *st);

#endif