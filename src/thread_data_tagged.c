#include <stdlib.h>
#include <glib.h>
#include "thread_data.h"

void* create_impl_specific_thread_data(thread_data_init_args_t args) {
    (void)args; // suppress unused parameter warning
    GSList** rlist = calloc(1, sizeof(GSList*));
    *rlist = NULL;
    return rlist;
}

void delete_impl_specific_thread_data(void* impl_data){
    GSList** rlistp = (GSList**)impl_data;
    GSList* rlist = *rlistp;
    // Free all retired stack nodes stored in the list.
    for (GSList* it = rlist; it != NULL; it = it->next) {
        free(it->data);
    }
    // Free the GSList nodes themselves.
    g_slist_free(rlist);
    free(rlistp);
    }

