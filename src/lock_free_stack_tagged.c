#include <stdlib.h>
#include <stdio.h>
#include <stdatomic.h>
#include "tagged.h"

typedef struct node_t {
    int key;
    struct node_t* next;
} node_t;

typedef struct pointer_t {
    node_t* node;
    long num;
} pointer_t;

typedef struct lf_stack_t {
    _Atomic pointer_t top;
} lf_stack_t;


node_t* new_node(GSList** rlist, int key) {
    // First try to reuse a node from the stack-specific free list (rlist)
    if (*rlist != NULL) {
        GSList* head = *rlist;
        node_t* a = (node_t*)head->data;
        // Remove the head element from the GSList
        *rlist = g_slist_delete_link(*rlist, head);
        // Reinitialize the recycled node
        a->key = key;
        a->next = NULL;
        return a;
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
    GSList** rlist = (GSList**)arg;
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
    } while (!atomic_compare_exchange_weak(&s->top, &old, new));
}

int pop(lf_stack_t* s, void* arg){
    GSList** rlist = (GSList**)arg;

    pointer_t old;
    pointer_t new;
    
    old = atomic_load(&s->top);

    do {
        if (old.node == NULL)
            return 0;
        new.num = old.num+1;
        new.node = old.node->next;
    } while(!atomic_compare_exchange_weak(&s->top, &old, new));
    //*rlist = g_slist_prepend(*rlist, old.node);
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
    posix_memalign((void**)&stack, 16, sizeof(lf_stack_t));
    memset(stack, 0, sizeof(*stack));
    
    
    pointer_t p = { .node = NULL, .num = 0 };
    atomic_init(&stack->top, p);
    return stack;
}

void delete_stack(lf_stack_t* stack){
    pointer_t top = atomic_load(&stack->top);
    node_t* cur = top.node;
    while (cur) {
        node_t* next = cur->next;
        free(cur);
        cur = next;
    }
    free(stack);
}
