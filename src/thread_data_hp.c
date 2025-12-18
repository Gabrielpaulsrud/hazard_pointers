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

void delete_impl_specific_thread_data(void* impl_data) {
    return;
    if (!impl_data) return;
    hp_thread_data_t* hpd = (hp_thread_data_t*) impl_data;


    list_free(hpd->rlist);
    free(hpd);

    // free(freelistp);
}
