#include <stdio.h>
#include "lock_free_stack.h"
#include <pthread.h>

int PUSHES = 10000;
int POPS =   10000;
int n_push_threads = 1000;
int n_pop_threads = 1000;
typedef struct { lf_stack_t* stack; int id; } pop_arg_t;

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
    // elem_t* elem;
    for (int i = 0; i < POPS; i++) {
        pop(stack);
        
        // int id = p->id;
        // if (elem != NULL){
        //     printf("[%d] popped %d\n", id, elem->key);
        // }
        // else {
        //     printf("[%d] popped NULL\n", id);
        // }
    }
    return NULL;
}

int main(){
    lf_stack_t* stack = init_stack();

    pthread_t push_threads[n_push_threads];
    pthread_t pop_threads[n_pop_threads];
    pop_arg_t pop_args[n_pop_threads];

    for (int i = 0; i < n_push_threads; i++) {
        pthread_create(&push_threads[i], NULL, push_task, stack);
    }
    for (int i = 0; i < n_pop_threads; i++) {
        pop_args[i].stack = stack;
        pop_args[i].id = i;
        pthread_create(&pop_threads[i], NULL, pop_task, &pop_args[i]);
    }
    for (int i = 0; i < n_push_threads; i++) {
        pthread_join(push_threads[i], NULL);
    }
    for (int i = 0; i < n_pop_threads; i++) {
        pthread_join(pop_threads[i], NULL);
    }
    return 0;
}