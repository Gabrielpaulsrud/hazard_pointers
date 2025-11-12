#include <stdio.h>
#include "lock_free_stack.h"
#include <pthread.h>
#include <stdatomic.h>

// int PUSHES = 10;
// int POPS =   10;
// int n_push_threads = 100;
// int n_pop_threads = 100;
int PUSHES = 30000;
int POPS =   30000;
int n_push_threads = 200;
int n_pop_threads = 200;
typedef struct { lf_stack_t* stack; int id; _Atomic unsigned long* pop_sum;} pop_arg_t;

typedef struct { lf_stack_t* stack; int id; } thread_arg_t;

void* push_task(void* arg){
    lf_stack_t* stack = (lf_stack_t*)arg;
    for (int i = 0; i < PUSHES; i++) {
        push(stack, i);
    }
    return NULL;
}

void* pop_task(void* arg){
    pop_arg_t* p = (pop_arg_t*)arg;
    lf_stack_t* stack = p->stack;
    _Atomic unsigned long* sum = p->pop_sum;
    for (int i = 0; i < POPS; i++) {
        int n = pop(stack);
        atomic_fetch_add(sum, n);

    }
    return NULL;
}

int main(){
    lf_stack_t* stack = init_stack();

    pthread_t push_threads[n_push_threads];
    pthread_t pop_threads[n_pop_threads];
    pop_arg_t pop_args[n_pop_threads];

    _Atomic unsigned long pop_sum = 0;

    for (int i = 0; i < n_push_threads; i++) {
        pthread_create(&push_threads[i], NULL, push_task, stack);
    }
    for (int i = 0; i < n_pop_threads; i++) {
        pop_args[i].stack = stack;
        pop_args[i].id = i;
        pop_args[i].pop_sum = &pop_sum;
        pthread_create(&pop_threads[i], NULL, pop_task, &pop_args[i]);
    }
    for (int i = 0; i < n_push_threads; i++) {
        pthread_join(push_threads[i], NULL);
    }
    for (int i = 0; i < n_pop_threads; i++) {
        pthread_join(pop_threads[i], NULL);
    }

    unsigned long remainder = sum(stack);
    unsigned long expected_sum = (unsigned long)n_push_threads * (unsigned long)PUSHES * (PUSHES - 1) / 2;
    if (expected_sum != pop_sum + remainder) {
        printf("INVARIANT FAILED\n");
        printf("pop sum   %lu\nstack sum %lu\nexpected  %lu\n", pop_sum, remainder, expected_sum);
    }
    else {
        printf("PASSED\nTotal sum of pushes %lu\n", pop_sum+remainder);
    }
    return 0;
}