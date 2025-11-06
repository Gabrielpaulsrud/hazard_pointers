#ifndef LOCK_FREE_STACK_H
#define LOCK_FREE_STACK_H

typedef struct elem_t {
    int key;
    struct elem_t* next;
} elem_t;

typedef struct lf_stack_t {
    elem_t* head;
} lf_stack_t;

void push(lf_stack_t* stack, int key);
elem_t* pop(lf_stack_t* stack);
lf_stack_t* init_stack();

#endif