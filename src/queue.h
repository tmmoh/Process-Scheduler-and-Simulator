#ifndef _QUEUE_H_
#define _QUEUE_H_ 

#include <stdlib.h>

typedef struct node node_t;
struct node {
    void* data;
    node_t* next;
};

typedef struct queue {
    node_t* head;
    node_t* tail;
    size_t len;
} queue_t;

queue_t* new_queue();

void* dequeue(queue_t* queue);

void enqueue(queue_t* queue, void* data);

int queue_empty(queue_t* queue);

void queue_free(queue_t* q, void (*data_free)(void* data));

#endif