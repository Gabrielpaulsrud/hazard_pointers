#include "linked_list.h"

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

void* init_hp(int N, int K);

void retireNode(void* p, hp_thread_data_t* hpd);