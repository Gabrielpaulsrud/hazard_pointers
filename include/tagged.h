#include <glib.h>
typedef struct {
    GSList* rlist;   // retired nodes for this thread
} tagged_thread_data_t;
