#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <stdatomic.h>
#include <time.h>
#include <string.h>
#include <errno.h>
#ifdef STACK_HP
#include "hp.h"
#endif
#include "lock_free_stack.h"
// #include "tagged.h"
#include "thread_data.h"
#include <locale.h>


// #define TEST

int PUSHES = 10000000;
int POPS =   10000000;
int n_push_threads = 4;
int n_pop_threads = 4;
static const int MAX_NODES_IN_RETIRE = 2;
static const int K = 2; //Max number of hazard pointers used by a thread

typedef struct {
    lf_stack_t* stack;
    int id;
    _Atomic unsigned long* pop_sum;
    void* impl_data;
} thread_arg_t;

static double monotonic_seconds(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (double)ts.tv_sec + (double)ts.tv_nsec / 1e9;
}

void* push_task(void* arg){
    thread_arg_t* args = (thread_arg_t*)arg;
    lf_stack_t* stack = args->stack;
    for (int i = 0; i < PUSHES; i++) {
        push(stack, i, args->impl_data);
    }
    return NULL;
}

void* pop_task(void* arg){
    thread_arg_t* args = (thread_arg_t*)arg;
    lf_stack_t* stack = args->stack;
    _Atomic unsigned long* sum = args->pop_sum;
    for (int i = 0; i < POPS; i++) {
        int n = pop(stack, args->impl_data);
        #ifdef TEST
            atomic_fetch_add(sum, n);
        #endif
    }
    return NULL;
}

int main(void){
    lf_stack_t* stack = init_stack();

    pthread_t push_threads[n_push_threads];
    pthread_t pop_threads[n_pop_threads];
    thread_arg_t thread_args[n_pop_threads+n_push_threads];

    _Atomic unsigned long pop_sum = 0;
    void** hp_record = NULL;
#ifdef STACK_HP
    hp_record = init_hp(n_push_threads + n_pop_threads, K);
#endif

    // Set thread args
    for (int i = 0; i < n_push_threads + n_pop_threads; i++){
        thread_args[i].stack = stack;
        thread_args[i].pop_sum = &pop_sum;
        thread_data_init_args_t init = {
            .hps = hp_record,
            .rlist = NULL,
            .idx = i,
            .r = 0,
            .max_r = MAX_NODES_IN_RETIRE,
            .K = K,
            .n_threads = n_push_threads + n_pop_threads
        };

        thread_args[i].impl_data = create_impl_specific_thread_data(init);
    }

    double start_time = monotonic_seconds();

    setlocale(LC_NUMERIC, "");

    for (int i = 0; i < n_push_threads; i++) {
        int ret = pthread_create(&push_threads[i], NULL, push_task, &thread_args[i]);
        if (ret != 0) {
            fprintf(stderr, "pthread_create(push %d) failed: %s\n",
                    i, strerror(ret));
            exit(1);
        }
    }
    
    for (int j = 0; j < n_pop_threads; j++) {
        int i = n_push_threads + j;
        int ret = pthread_create(&pop_threads[j], NULL, pop_task, &thread_args[i]);
        if (ret != 0) {
            fprintf(stderr, "pthread_create(pop %d) failed: %s\n",
                    j, strerror(ret));
            exit(1);
        }
    }

    for (int i = 0; i < n_push_threads; i++) {
        int ret = pthread_join(push_threads[i], NULL);
        if (ret != 0) {
            fprintf(stderr, "pthread_join(push %d) failed: %s\n",
                    i, strerror(ret));
            exit(1);
        }
        free(thread_args[i].impl_data);
    }
    for (int j = 0; j < n_pop_threads; j++) {
        int ret = pthread_join(pop_threads[j], NULL);
        if (ret != 0) {
            fprintf(stderr, "pthread_join(pop %d) failed: %s\n",
                    j, strerror(ret));
            exit(1);
        }
    }
    for (int j = 0; j < n_pop_threads; j++) {
        int i = n_push_threads + j;
        delete_impl_specific_thread_data(thread_args[i].impl_data);
    }

    
#ifdef STACK_HP
    free(hp_record);
#endif
    #ifdef TEST
    unsigned long remainder = sum(stack);
    unsigned long expected_sum = (unsigned long)n_push_threads * (unsigned long)PUSHES * (PUSHES - 1) / 2;
    if (expected_sum != pop_sum + remainder) {
        printf("INVARIANT FAILED\n");
        printf("pop sum   %'lu\nstack sum %'lu\nexpected  %'lu\n", pop_sum, remainder, expected_sum);
    }
    else {
        printf("PASSED\nTotal sum of pushes %'lu\n", pop_sum+remainder);
        printf("Popped: %'lu\nLeft in stack: %'lu\n", pop_sum, remainder);
    }
    #endif
    double end_time = monotonic_seconds();
    printf("Runtime: %.6f seconds\n", end_time - start_time);

    printf("Taotal number of pushes + pops %'llu\n", (unsigned long long)n_pop_threads*POPS+n_push_threads*PUSHES);
    delete_stack(stack);

    return 0;
}
