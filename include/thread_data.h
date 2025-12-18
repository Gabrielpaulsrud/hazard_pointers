// #include <glib.h>
#include "utils/list.h"
typedef struct thread_data_init_args {
    void** hps;
    list_node_t* rlist;
    int idx;
    int r;
    int max_r;
    int K;
    int n_threads;
} thread_data_init_args_t;

void* create_impl_specific_thread_data(thread_data_init_args_t args);

void delete_impl_specific_thread_data(void* impl_data);

