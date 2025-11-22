typedef struct elem_t {
    int key;
    struct elem_t* next;
} elem_t;

typedef struct lf_stack_t {
    elem_t* head;
} lf_stack_t;

void push(lf_stack_t* stack, int key);
int pop(lf_stack_t* stack, void* arg);
unsigned long sum(lf_stack_t* stack); //Not lock free
lf_stack_t* init_stack(void);
