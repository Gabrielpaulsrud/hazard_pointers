typedef struct node_t {
    int key;
    struct node_t* next;
} node_t;

typedef struct pointer_t {
    node_t* node;
    long num;
} pointer_t;

typedef struct lf_stack_t lf_stack_t;

void push(lf_stack_t* stack, int key, void* arg);
int pop(lf_stack_t* stack, void* arg);
unsigned long sum(lf_stack_t* stack); //Not lock free
lf_stack_t* init_stack(void);
void delete_stack(lf_stack_t* stack);
