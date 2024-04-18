
#include "linkedlist.h"
#include <assert.h>
#include <stdlib.h>

// Creates and returns a new empty list
list_t *new_list() {
    list_t *l = malloc(sizeof(*l));
    assert(l);

    l->head = NULL;
    l->tail = NULL;
    l->len = 0;

    return l;
}

// Removes the data at the head of the provided list
// Returns a pointer to the data
void *list_remove_head(list_t *list) {
    if (list->len < 1) {
        // Nothing to remove
        return NULL;
    }

    // Remove the data from head
    node_t *node = list->head;
    void *data = node->data;
    list->head = node->next;

    // If there's still still elements in the list, update the head's prev
    // pointer
    if (list->head)
        list->head->prev = NULL;

    list->len -= 1;

    free(node);

    return data;
}

// Adds data to the tail of the provided list
void list_add_tail(list_t *list, void *data) {
    node_t *new = malloc(sizeof(*new));
    assert(new);

    new->data = data;
    new->next = NULL;
    new->prev = NULL;

    if (list->len <= 0) {
        // First element in the list
        list->head = list->tail = new;
        list->len = 1;
        return;
    }

    new->prev = list->tail;
    list->tail->next = new;
    list->tail = new;
    list->len += 1;
}

// Frees the list and all remaining data in it using the provided data_free
// function
void list_free(list_t *l, void (*data_free)(void *data)) {
    node_t *curr = l->head;
    while (curr) {
        node_t *prev = curr;
        data_free(curr->data);
        curr = curr->next;
        free(prev);
    }
    free(l);
}