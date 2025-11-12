struct hp_t{
    void* p;
};

typedef struct hp_t hp_t;

struct hp_thread_data_t{
    hp_t* hps;
    void **rlist;
    int i; //index of thread
    int r;
    int max_r;
};

typedef struct hp_thread_data_t hp_thread_data_t;

hp_t* init_hp(int N, int K);