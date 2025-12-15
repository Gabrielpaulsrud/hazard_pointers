#include <stdlib.h>
#include <stdio.h>
#include "hp.h"

void** init_hp(int N, int K){
    //N = number of threads
    //K = number of pointers per thread (typically 1 or 2)
    void** records = calloc(N*K, sizeof(void*));
    return records;
}

void scan(hp_thread_data_t* hpd){
    //Stage 1
    void* curr;
    GHashTable* p_set = g_hash_table_new(g_direct_hash, g_direct_equal);
    for (int i = 0; i < hpd->K * hpd->n_threads; i++) {
        curr = hpd->hps[i];
        if(curr != NULL) {
            g_hash_table_add(p_set, curr);
        }
    }

    //Stage 2
    hpd->r = 0;
    GSList* tmp_list = hpd->rlist;
    hpd->rlist = NULL;

    for (GSList* it = tmp_list; it != NULL; it = it->next){
        void* node = it->data;

        if (g_hash_table_contains(p_set, node)) {
            hpd->rlist = g_slist_prepend(hpd->rlist, node);
            hpd->r += 1;
        }
        else {
            free(node);
        }
    }
    
    g_slist_free(tmp_list);
    g_hash_table_destroy(p_set);
}

void retireNode(void* p, hp_thread_data_t* hpd){
    hpd->rlist = g_slist_prepend(hpd->rlist, p);
    hpd->r += 1;
    if (hpd->r > hpd->max_r){
        scan(hpd);
    }
}

void init_hpd(hp_thread_data_t* hpd, void** hps, GSList* rlist, int idx, int r, int max_r, int K, int n_threads) {
    hpd->hps = hps;
    hpd->rlist = rlist;
    hpd->idx = idx;
    hpd->r = r;
    hpd->max_r = max_r;
    hpd->K = K;
    hpd->n_threads = n_threads;
}

