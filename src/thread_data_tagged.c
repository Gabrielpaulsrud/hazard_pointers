#include <stdlib.h>
#include <glib.h>
#include "thread_data.h"

void* create_impl_specific_thread_data(thread_data_init_args_t args) {
    (void)args; // suppress unused parameter warning
    GSList** rlist = calloc(1, sizeof(GSList*));
    *rlist = NULL;
    return rlist;
}
