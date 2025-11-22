#include <stdlib.h>
#include "hp.h"
#include "thread_data.h"

void* create_impl_specific_thread_data(thread_data_init_args_t args) {
    hp_thread_data_t* hpd = calloc(1, sizeof(*hpd));
    if (!hpd) {
        return NULL;
    }

    hpd->hps       = args.hps;
    hpd->rlist     = args.rlist;
    hpd->idx       = args.idx;
    hpd->r         = args.r;
    hpd->max_r     = args.max_r;
    hpd->K         = args.K;
    hpd->n_threads = args.n_threads;

    return hpd;
}
