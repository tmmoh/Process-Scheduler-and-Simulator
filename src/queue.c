#include <stdlib.h>
#include <assert.h>
#include "queue.h"
#include "linkedlist.h"


queue_t* new_queue() {
    return (queue_t*) new_list();
}


void* dequeue(queue_t* queue) {
    return list_remove_head((list_t*) queue);
}


void enqueue(queue_t* queue, void* data) {
    list_add_tail((list_t*) queue, data);
}

void requeue_head(queue_t* q) {
    enqueue(q, dequeue(q));
    /* q->tail = q->head;
    if (q->head->next) q->head = q->head->next;
    q->tail->next = NULL; */
}

void requeue(queue_t* q, node_t* node) {
    if (q->tail == node) return;

    if (q->head == node) return requeue_head(q);

    node->prev->next = node->next;
    node->next->prev = node->prev;

    enqueue(q, node->data);
    free(node);
}

void queue_free(queue_t* q, void (*data_free)(void* data)) {
    list_free((list_t*) q, data_free);
}