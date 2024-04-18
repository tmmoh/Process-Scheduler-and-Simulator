#ifndef _QUEUE_H_
#define _QUEUE_H_

#include "linkedlist.h"
#include <stdlib.h>

// Queue is a wrapper class over a doubly-linked list
// Exposes only certain functionality, as well as extra methods
typedef list_t queue_t;

// Creates and returns a new queue
queue_t *new_queue();

// Deletes and returns the element at the head of the queue
void *dequeue(queue_t *queue);

// Adds an element to the end of the queue
void enqueue(queue_t *queue, void *data);

// Places the element at the head of a queue to it's tail
void requeue_head(queue_t *q);

// Places a node in the queue at the tail
void requeue(queue_t *q, node_t *node);

// Removes a node in the queue and returns the data
void *queue_remove(queue_t *q, node_t *node);

// Frees a queue and all data it holds using the provided data_free function
void queue_free(queue_t *q, void (*data_free)(void *data));

#endif