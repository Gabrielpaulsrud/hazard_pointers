#include <stdlib.h>
#include <stdio.h>
#include <stdatomic.h>
#include <hp.h>

#define FREE 1

typedef struct elem_t {
    int key;
    struct elem_t* next;
} elem_t;

typedef struct lf_stack_t {
    _Atomic(elem_t*) top;
} lf_stack_t;

void push(lf_stack_t* stack, int key){
    elem_t* elem = malloc(sizeof(elem_t));
    elem->key=key;
    elem->next = atomic_load(&stack->top);
    while(!atomic_compare_exchange_weak(&stack->top, &elem->next, elem))
        ;
}

int pop(lf_stack_t* stack, hp_thread_data_t* hpd){
    elem_t* old;
    int key;
    
    old = atomic_load(&stack->top);
    while(old != NULL && !atomic_compare_exchange_weak(&stack->top, &old, old->next))
    ;
    if (old == NULL) {
        return 0;
    }
    key = old->key;
    retire();
    return key;
}

//Not lock free
unsigned long sum(lf_stack_t* stack){
    unsigned long sum = 0;
    elem_t* current = stack->top;
    while (current != NULL) {
        sum += (unsigned long) current->key;
        current = current->next;
    }
    return sum;
}

lf_stack_t* init_stack(){
    lf_stack_t* stack = malloc(sizeof(lf_stack_t));
    stack->top = NULL;
    return stack;
}

