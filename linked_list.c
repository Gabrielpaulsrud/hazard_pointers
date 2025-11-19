#include <stdlib.h>
#include <stdbool.h>

typedef struct ll_node {
    void* value;
    struct ll_node* next;
} ll_node_t;

typedef struct linked_list {
    ll_node_t* head;
} linked_list_t;

// Create a new empty list
linked_list_t* ll_create(void) {
    linked_list_t* l = malloc(sizeof(linked_list_t));
    l->head = NULL;
    return l;
}

// Push to front
void ll_push_front(linked_list_t* l, void* value) {
    ll_node_t* n = malloc(sizeof(ll_node_t));
    n->value = value;
    n->next = l->head;
    l->head = n;
}

// Pop from front. Returns NULL if list empty.
void* ll_pop_front(linked_list_t* l) {
    if (!l->head) return NULL;
    ll_node_t* n = l->head;
    l->head = n->next;
    void* v = n->value;
    free(n);
    return v;
}

bool contains(linked_list_t* l, void* value) {
    for (ll_node_t* n = l->head; n != NULL; n = n->next) {
        if (n->value == value) {
            return true;
        }
    }
    return false;
}

// Free all nodes (does NOT free stored values)
void ll_destroy(linked_list_t* l) {
    ll_node_t* n = l->head;
    while (n) {
        ll_node_t* next = n->next;
        free(n);
        n = next;
    }
    free(l);
}
