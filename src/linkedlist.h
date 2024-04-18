#ifndef _LINKED_LIST_H
#define _LINKED_LIST_H

#include <stdlib.h>

typedef struct node node_t;

// Struct for a node in a doubly-linked list
struct node {
    void* data;
    node_t* prev;
    node_t* next;
};

// Struct for a doubly-linked list
typedef struct list {
    node_t* head;
    node_t* tail;
    size_t len;
} list_t;

// Creates and returns a new empty list
list_t* new_list();

// Removes the data at the head of the provided list
// Returns a pointer to the data
void* list_remove_head(list_t* list);

// Adds data to the tail of the provided list
void list_add_tail(list_t* list, void* data);

// Frees the list and all remaining data in it using the provided data_free function
void list_free(list_t* l, void (*data_free)(void* data));

#endif