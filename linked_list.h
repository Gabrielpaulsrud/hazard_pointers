#include <stddef.h>
#include <stdbool.h>

typedef struct ll_node {
    void* value;
    struct ll_node* next;
} ll_node_t;

typedef struct linked_list {
    ll_node_t* head;
} linked_list_t;

// Create a new empty list. Returns NULL on allocation failure.
linked_list_t* ll_create(void);

// Push a value at the front of the list. Does nothing if l is NULL.
void ll_push_front(linked_list_t* l, void* value);

// Pop from the front. Returns NULL if the list is empty or l is NULL.
void* ll_pop_front(linked_list_t* l);

bool contains(linked_list_t* l, void* value);

// Destroy the list and free all nodes.
// Does NOT free the stored values, only the list nodes and the list itself.
void ll_destroy(linked_list_t* l);