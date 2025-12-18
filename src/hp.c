#include <stdlib.h>
#include <stdio.h>
#include "hp.h"
#include "utils/list.h"


void** init_hp(int N, int K){
    //N = number of threads
    //K = number of pointers per thread (typically 1 or 2)
    void** records = calloc(N*K, sizeof(void*));
    return records;
}

int in_array(void** arr, int len, void* val){
    for(int i = 0; i < len; i++){
        if (arr[i] == val){
            return 1;
        }
    }
    return 0;
}

void scan(hp_thread_data_t* hpd){
    //Stage 1
    // void* curr;
    // GHashTable* p_set = g_hash_table_new(g_direct_hash, g_direct_equal);
    // for (int i = 0; i < hpd->K * hpd->n_threads; i++) {
    //     curr = hpd->hps[i];
    //     if(curr != NULL) {
    //         g_hash_table_add(p_set, curr);
    //     }
    // }
    int MAX_HP = hpd->K * hpd->n_threads;
    void* hp_arr[MAX_HP];
    int hp_cnt = 0;

    for (int i = 0; i < MAX_HP; i++) {
        if (hpd->hps[i]){
            hp_arr[hp_cnt] = hpd->hps[i];
            hp_cnt++;
        }
    }
    hp_arr[hp_cnt] = NULL;


    //Stage 2
    hpd->r = 0;
    list_node_t* tmp_list = hpd->rlist;
    hpd->rlist = NULL;

    while (tmp_list != NULL) {
        if (in_array(hp_arr, hp_cnt, tmp_list->data)){
            list_push(&hpd->rlist, tmp_list->data);
            hpd->r += 1;
        }
        else {
            free(tmp_list->data);
        }
        tmp_list = tmp_list->next;
    }

    // list_free(tmp_list);
    // for (list_node_t* it = tmp_list; it != NULL; it = it->next){
    //     void* node = it->data;

    //     if (g_hash_table_contains(p_set, node)) {
    //         hpd->rlist = g_slist_prepend(hpd->rlist, node);
    //         hpd->r += 1;
    //     }
    //     else {
    //         free(node);
    //     }
    // }
    
    // g_slist_free(tmp_list);
    // g_hash_table_destroy(p_set);
}

void retireNode(void* p, hp_thread_data_t* hpd){
    // hpd->rlist = g_slist_prepend(hpd->rlist, p);
    list_push(&hpd->rlist, p);
    hpd->r += 1;
    if (hpd->r > hpd->max_r){
        scan(hpd);
    }
}

void init_hpd(hp_thread_data_t* hpd, void** hps, list_node_t* rlist, int idx, int r, int max_r, int K, int n_threads) {
    hpd->hps = hps;
    hpd->rlist = rlist;
    hpd->idx = idx;
    hpd->r = r;
    hpd->max_r = max_r;
    hpd->K = K;
    hpd->n_threads = n_threads;
}

