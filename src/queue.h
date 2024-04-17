#ifndef _QUEUE_H_
#define _QUEUE_H_ 

#include <stdlib.h>
#include "linkedlist.h"

typedef list_t queue_t;

queue_t* new_queue();

void* dequeue(queue_t* queue);

void enqueue(queue_t* queue, void* data);

void requeue_head(queue_t* q);

void queue_free(queue_t* q, void (*data_free)(void* data));

#endif