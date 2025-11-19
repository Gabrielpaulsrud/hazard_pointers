#include <stdlib.h>
#include "linked_list.h"
#include <stdio.h>


typedef struct hp_t hp_t;

struct hp_thread_data_t{
    void** hps;
    linked_list_t* rlist;
    int idx;
    int r;
    int max_r;
    int K;
    int n_threads;
};

typedef struct hp_thread_data_t hp_thread_data_t;

void** init_hp(int N, int K){
    //N = number of threads
    //K = number of pointers per thread (typically 1 or 2)
    void** records = calloc(N*K, sizeof(void*));
    return records;
}

void scan(hp_thread_data_t* hpd){
    //Stage 1
    // void* head = hpd->hps[0];
    void* curr;
    linked_list_t* p_list = ll_create();
    for (int i = 0; i < hpd->K * hpd->n_threads; i++) {
        curr = hpd->hps[i];
        if(curr != NULL) {
            ll_push_front(p_list, curr);
        }
    }

    //Stage 2
    hpd->r = 0;
    linked_list_t* tmp_list = ll_create();
    void* front = ll_pop_front(hpd->rlist);
    while (front != NULL){
        ll_push_front(tmp_list, front);
        front = ll_pop_front(hpd->rlist);
    }

    while(front != NULL) {
        front = ll_pop_front(tmp_list);
        if(contains(p_list, front)){
            ll_push_front(hpd->rlist, front);
            hpd->r += 1;
        }
        else{
            free(front);
        }
    }
    ll_destroy(p_list);
    ll_destroy(tmp_list);
}

void retireNode(void* p, hp_thread_data_t* hpd){
    ll_push_front(hpd->rlist, p);
    hpd->r += 1;
    // printf("retire, setting r to %d\n", hpd->r);
    if (hpd->r > hpd->max_r){
        // printf("Calling scan with r = %d\n", hpd->r);
        scan(hpd);
    }
}
