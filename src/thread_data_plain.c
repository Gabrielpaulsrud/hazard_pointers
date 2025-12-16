#include "thread_data.h"
#include <stdlib.h>

void* create_impl_specific_thread_data(thread_data_init_args_t args) {
    (void) args;
    return NULL;
}

void delete_impl_specific_thread_data(void* impl_data){
    (void) impl_data;
    return;
}
