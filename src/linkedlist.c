
#include <stdlib.h>
#include <assert.h>
#include "linkedlist.h"


list_t* new_list() {
    list_t* l = malloc(sizeof(*l));
    assert(l);

    l->head = NULL;
    l->tail = NULL;
    l->len = 0;

    return l;
}


void* list_remove_head(list_t* list) {
    if (list->len < 1) {
        return NULL;
    } 

    node_t* node = list->head;
    void* data = node->data;
    list->head = node->next;

    list->len -= 1;

    free(node);

    return data;
}


void list_add_tail(list_t* list, void* data) {
    node_t* new = malloc(sizeof(*new));
    assert(new);

    new->data = data;
    new->next = NULL;

    if (list->len <= 0) {
        list->head = list->tail = new;
        list->len = 1;
        return;
    }

    list->tail->next = new;
    list->tail = new;
    list->len = list->len + 1;
}

void list_free(list_t* l, void (*data_free)(void* data)) {
    while(l->len > 0) {
        data_free(list_remove_head(l));
    }
    free(l);
}