#include <stdlib.h>
#include <assert.h>
#include "queue.h"


queue_t* new_queue() {
    queue_t* q = malloc(sizeof(*q));
    assert(q);

    q->head = NULL;
    q->tail = NULL;
    q->len = 0;

    return q;
}


void* dequeue(queue_t* queue) {
    if (queue->len < 1) {
        return NULL;
    } 

    node_t* node = queue->head;
    void* data = node->data;
    queue->head = node->next;

    queue->len -= 1;

    free(node);

    return data;
}


void enqueue(queue_t* queue, void* data) {
    node_t* new = malloc(sizeof(*new));
    assert(new);

    new->data = data;
    new->next = NULL;

    if (queue->len == 0) {
        queue->head = queue->tail = new;
        queue->len = 1;
        return;
    }


    queue->tail->next = new;
    queue->tail = new;
    queue->len = queue->len + 1;
}
