#include <stdlib.h>
#include <stdio.h>
#include <stdatomic.h>
#include "hp.h"

#define FREE 1

typedef struct node_t {
    int key;
    struct node_t* next;
} node_t;

typedef struct lf_stack_t {
    _Atomic(node_t*) top;
} lf_stack_t;

void push(lf_stack_t* stack, int key, void* arg){
    (void)arg;
    node_t* a = malloc(sizeof(node_t));
    a->key=key;
    a->next = atomic_load(&stack->top);
    while(!atomic_compare_exchange_weak(&stack->top, &a->next, a))
        ;
}

int pop(lf_stack_t* stack, void* arg){
    hp_thread_data_t* hpd = arg;
    node_t* old;
    node_t* next;
    int key;
    while(1){
        old = atomic_load(&stack->top);
        if (old == NULL) {
            return 0;
        }
        hpd->hps[hpd->idx] = old;
        if(hpd->hps[hpd->idx] != atomic_load(&stack->top)) {
            continue;
        }
        next = old->next;
        hpd->hps[hpd->idx+1] = next;

        //Why would this be needed?
        // It says in the paper...
        if(hpd->hps[hpd->idx] != atomic_load(&stack->top)) {
            continue;
        }
        
        if (atomic_compare_exchange_weak(&stack->top, &old, next)){
            break;
        }

    }
    
    key = old->key;
    retireNode(old, hpd);
    hpd->hps[hpd->idx] = NULL;
    hpd->hps[hpd->idx+1] = NULL;
    return key;
}

//Not lock free
unsigned long sum(lf_stack_t* stack){
    unsigned long sum = 0;
    node_t* current = stack->top;
    while (current != NULL) {
        sum += (unsigned long) current->key;
        current = current->next;
    }
    return sum;
}

lf_stack_t* init_stack(void){
    lf_stack_t* stack = malloc(sizeof(lf_stack_t));
    stack->top = NULL;
    return stack;
}
