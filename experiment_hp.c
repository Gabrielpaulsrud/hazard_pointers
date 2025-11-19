#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <stdatomic.h>
#include "hp.h"
#include "lock_free_stack_hp.h"

int PUSHES = 100000;
int POPS =   100000;
int n_push_threads = 1000;
int n_pop_threads = 1000;
int MAX_NODES_IN_RETIRE = 10;
// int PUSHES = 30000;
// int POPS =   30000;
// int n_push_threads = 200;
// int n_pop_threads = 200;

typedef struct {
    lf_stack_t* stack;
    hp_thread_data_t* hpd;
    int id;
    _Atomic unsigned long* pop_sum;
} thread_arg_t;

void* push_task(void* arg){
    thread_arg_t* args = (thread_arg_t*)arg;
    lf_stack_t* stack = args->stack;
    for (int i = 0; i < PUSHES; i++) {
        push(stack, i);
    }
    return NULL;
}

void* pop_task(void* arg){
    thread_arg_t* args = (thread_arg_t*)arg;
    lf_stack_t* stack = args->stack;
    _Atomic unsigned long* sum = args->pop_sum;
    for (int i = 0; i < POPS; i++) {
        int n = pop(stack, args->hpd);
        atomic_fetch_add(sum, n);
    }
    return NULL;
}

int main(){
    lf_stack_t* stack = init_stack();

    pthread_t push_threads[n_push_threads];
    pthread_t pop_threads[n_pop_threads];
    thread_arg_t thread_args[n_pop_threads+n_push_threads];

    _Atomic unsigned long pop_sum = 0;
    int K = 2;
    void** hp_record = init_hp(n_push_threads + n_pop_threads, K);
    // Set thread args
    for (int i = 0; i < n_push_threads + n_pop_threads; i++){
        thread_args[i].stack = stack;
        thread_args[i].hpd = calloc(1, sizeof(hp_thread_data_t));
        thread_args[i].hpd->hps = hp_record;

        thread_args[i].hpd->idx = i*K;
        thread_args[i].hpd->r = 0;
        thread_args[i].hpd->rlist = calloc(MAX_NODES_IN_RETIRE, sizeof(void*));
        thread_args[i].hpd->max_r = MAX_NODES_IN_RETIRE;
        thread_args[i].pop_sum = &pop_sum;
    }

    for (int i = 0; i < n_push_threads; i++) {
        pthread_create(&push_threads[i], NULL, push_task, &thread_args[i]);
    }
    
    for (int j = 0; j < n_pop_threads; j++) {
        int i = n_push_threads + j;
        pthread_create(&pop_threads[j], NULL, pop_task, &thread_args[i]);
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
        printf("Popped: %lu\nLeft in stack: %lu\n", pop_sum, remainder);
    }
    return 0;
}