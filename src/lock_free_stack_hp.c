#include <stdlib.h>
#include <stdio.h>
#include <stdatomic.h>
#include "hp.h"
#include "lock_free_stack.h"

#include <locale.h>
_Atomic long long n_cas = 0;

typedef struct lf_stack_t {
    _Atomic(node_t*) top;
} lf_stack_t;

void push(lf_stack_t* stack, int key, void* arg){
    (void)arg;
    node_t* a = malloc(sizeof(node_t));
    a->key=key;
    a->next = atomic_load(&stack->top);
    do {
        #ifdef TEST
            atomic_fetch_add(&n_cas, 1);
        #endif
    } while(!atomic_compare_exchange_weak(&stack->top, &a->next, a));
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
        hpd->hps[hpd->idx*hpd->K] = old;
        if(hpd->hps[hpd->idx*hpd->K] != atomic_load(&stack->top)) {
            continue;
        }
        next = old->next;
        hpd->hps[hpd->idx*hpd->K+1] = next;

        //Why would this be needed?
        // It says in the paper...
        if(hpd->hps[hpd->idx*hpd->K] != atomic_load(&stack->top)) {
            continue;
        }
        #ifdef TEST
            atomic_fetch_add(&n_cas, 1);
        #endif
        if (atomic_compare_exchange_weak(&stack->top, &old, next)){
            break;
        }

    }
    key = old->key;
    retireNode(old, hpd);
    hpd->hps[hpd->idx*hpd->K] = NULL;
    hpd->hps[hpd->idx*hpd->K+1] = NULL;
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

void delete_stack(lf_stack_t* stack){
    setlocale(LC_NUMERIC, "");
    #ifdef TEST
    printf("%'llu cas\n", n_cas);
    #endif

    node_t* top = atomic_load(&stack->top);
    while (top) {
        node_t* next = top->next;
        free(top);
        top = next;
    }
    free(stack);
}
