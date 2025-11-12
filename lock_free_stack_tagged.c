#include <stdlib.h>
#include <stdio.h>
#include <stdatomic.h>

#define FREE 1

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
    atomic_long num;
} lf_stack_t;


node_t* new_node(int key) {
    node_t* a;
    
    a = calloc(1, sizeof(node_t));

    a->key = key;

    return a;
}

void push(lf_stack_t* s, int key) {
    node_t* a;
    pointer_t old;
    pointer_t new;
    
    a = new_node(key);
    old = atomic_load(&s->top);

    new.node = a;
    new.num = atomic_fetch_add(&s->num, 1);

    do {
        new.node->next = old.node;
    } while (!atomic_compare_exchange_weak(&s->top, &old, new));
}

int pop(lf_stack_t* s){
    int key;
    pointer_t old;
    pointer_t new;
    
    old = atomic_load(&s->top);
    new.num = atomic_fetch_add(&s->num, 1);

    do {
        if (old.node == NULL)
            break;
        else
            new.node = old.node->next;
    } while(!atomic_compare_exchange_weak(&s->top, &old, new));
    if (old.node == NULL) {
        return 0;
    }
    key = old.node -> key;
    free(old.node);
    return key;
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

lf_stack_t* init_stack() {
    lf_stack_t* stack = calloc(1, sizeof(lf_stack_t));
    pointer_t p = { .node = NULL, .num = 0 };
    atomic_init(&stack->top, p);
    atomic_init(&stack->num, 0);
    return stack;
}