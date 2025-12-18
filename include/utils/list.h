#pragma once

typedef struct list_node {
    void* data;
    struct list_node* next;
} list_node_t;

// prepend: O(1)
void list_push(list_node_t** head, void* data);

// pop front: O(1), returns NULL if empty
void* list_pop(list_node_t** head);

// free list nodes (does NOT free data)
void list_free(list_node_t* head);
