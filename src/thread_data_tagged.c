#include <stdlib.h>

#include "thread_data.h"
#include "lock_free_stack.h"  // brings in node_t

void* create_impl_specific_thread_data(thread_data_init_args_t args) {
    (void)args;

    // Per-thread intrusive free-list head (node_t linked via node_t::next)
    node_t** freelist = (node_t**)calloc(1, sizeof(node_t*));
    if (!freelist) return NULL;

    *freelist = NULL;
    return (void*)freelist;
}

void delete_impl_specific_thread_data(void* impl_data) {
    if (!impl_data) return;

    node_t** freelistp = (node_t**)impl_data;
    node_t* cur = *freelistp;

    while (cur) {
        node_t* next = cur->next;
        free(cur);
        cur = next;
    }

    free(freelistp);
}
