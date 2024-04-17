#ifndef _LINKED_LIST_H
#define _LINKED_LIST_H

#include <stdlib.h>

typedef struct node node_t;
struct node {
    void* data;
    node_t* next;
};

typedef struct list {
    node_t* head;
    node_t* tail;
    size_t len;
} list_t;


list_t* new_list();

void* list_remove_head(list_t* list);

void list_add_tail(list_t* list, void* data);

void list_free(list_t* l, void (*data_free)(void* data));

#endif