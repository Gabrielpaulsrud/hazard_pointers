#include <stdlib.h>
#include "utils/list.h"

void list_push(list_node_t** head, void* data) {
    list_node_t* n = malloc(sizeof(*n));
    n->data = data;
    n->next = *head;
    *head = n;
}

void* list_pop(list_node_t** head) {
    if (!*head) return NULL;
    list_node_t* n = *head;
    void* data = n->data;
    *head = n->next;
    free(n);
    return data;
}

void list_free(list_node_t* head) {
    while (head) {
        list_node_t* next = head->next;
        free(head);
        head = next;
    }
}
