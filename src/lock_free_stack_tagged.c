#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>
#include <stdio.h>
#include <stdatomic.h>
// #include "tagged.h"
#include <string.h>
#include "lock_free_stack.h"
#include <locale.h>
_Atomic long long n_cas = 0;

typedef struct pointer_t {
    node_t* node;
    long num;
} pointer_t;

typedef struct lf_stack_t {
    _Atomic pointer_t top;
} lf_stack_t;


node_t* new_node(node_t** rlist, int key) {
    // First try to reuse a node from the stack-specific free list (rlist)
    if (*rlist != NULL) {
        node_t* recycled = *rlist;
        // node_t* a = (node_t*)head->data;
        // Remove the head element from the GSList
        // *rlist = g_slist_delete_link(*rlist, head);
        *rlist = recycled->next;
        // Reinitialize the recycled node
        recycled->key = key;
        recycled->next = NULL;
        return recycled;
    }

    // Free list empty: fall back to fresh allocation
    node_t* a = calloc(1, sizeof(node_t));
    if (!a) {
        perror("calloc");
        exit(EXIT_FAILURE);
    }
    a->key = key;
    a->next = NULL;
    return a;
}

void push(lf_stack_t* s, int key, void* arg) {
    node_t** rlist = (node_t**)arg;
    node_t* a;
    pointer_t old;
    pointer_t new;
    
    a = new_node(rlist, key);
    old = atomic_load(&s->top);

    new.node = a;
    // new.num = atomic_fetch_add(&s->num, 1);
    
    do {
        new.node->next = old.node;
        new.num = old.num+1;
        #ifdef TEST
            atomic_fetch_add(&n_cas, 1);
        #endif
        } while (!atomic_compare_exchange_weak(&s->top, &old, new));
}

int pop(lf_stack_t* s, void* arg){
    node_t** rlist = (node_t**)arg;

    pointer_t old;
    pointer_t new;
    
    old = atomic_load(&s->top);

    do {
        if (old.node == NULL){
            return 0;
        }
        new.num = old.num+1;
        new.node = old.node->next;
        #ifdef TEST
            atomic_fetch_add(&n_cas, 1);
        #endif
    } while(!atomic_compare_exchange_weak(&s->top, &old, new));
    // *rlist = g_slist_prepend(*rlist, old.node);
    old.node->next = *rlist;
    *rlist = old.node;
    return old.node -> key;
}

//Not lock free
unsigned long sum(lf_stack_t* stack){
    unsigned long sum = 0;
    pointer_t top = atomic_load(&stack->top);
    node_t* current = top.node;
    while (current != NULL) {
        sum += (unsigned long) current->key;
        current = current->next;
    }
    return sum;
}

lf_stack_t* init_stack(void){
    //lf_stack_t* stack = calloc(1, sizeof(lf_stack_t));
    lf_stack_t* stack;
    //posix_memalign((void**)&stack, 16, sizeof(lf_stack_t));
    int ret = posix_memalign((void**)&stack, 16, sizeof(lf_stack_t));
    if (ret != 0) {
        fprintf(stderr,
            "posix_memalign failed (align=16, size=%zu): %s\n",
            sizeof(lf_stack_t),
            strerror(ret));
        return NULL;   // or exit(1), depending on your API
    }
    memset(stack, 0, sizeof(*stack));
    
    
    pointer_t p = { .node = NULL, .num = 0 };
    atomic_init(&stack->top, p);
    return stack;
}

void delete_stack(lf_stack_t* stack){
    setlocale(LC_NUMERIC, "");
    #ifdef TEST
    printf("%'llu cas\n", n_cas);
    #endif
    pointer_t top = atomic_load(&stack->top);
    node_t* cur = top.node;
    while (cur) {
        node_t* next = cur->next;
        free(cur);
        cur = next;
    }
    free(stack);
}
