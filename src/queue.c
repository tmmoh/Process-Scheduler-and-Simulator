#include "queue.h"
#include "linkedlist.h"
#include <assert.h>
#include <stdlib.h>

// Creates and returns a new queue
queue_t *new_queue() {
    return (queue_t *)new_list();
}

// Deletes and returns the element at the head of the queue
void *dequeue(queue_t *queue) {
    return list_remove_head((list_t *)queue);
}

// Adds an element to the end of the queue
void enqueue(queue_t *queue, void *data) {
    list_add_tail((list_t *)queue, data);
}

// Places the element at the head of a queue to it's tail
void requeue_head(queue_t *q) {
    if (q->len <= 1) {
        // Queue is empty or only has one element
        return;
    }

    node_t *node = q->head;

    q->head = q->head->next;
    q->head->prev = NULL;

    node->prev = q->tail;
    q->tail->next = node;
    q->tail = node;
    q->tail->next = NULL;
}

// Places a node in the queue at the tail
void requeue(queue_t *q, node_t *node) {
    // Node is already at the tail, nothing to do
    if (q->tail == node) {
        return;
    }
    // Node is at the head, just call requeue_head
    if (q->head == node) {
        return requeue_head(q);
    }
    node->prev->next = node->next;
    node->next->prev = node->prev;

    node->prev = q->tail;
    q->tail->next = node;
    q->tail = node;
    q->tail->next = NULL;
}

// Removes a node in the queue and returns the data
void *queue_remove(queue_t *q, node_t *node) {
    // Node is at the head, just call dequeue;
    if (q->head == node) {
        return dequeue(q);
    }
    void *data = node->data;
    q->len -= 1;

    // Node is at the tail, cut it off
    if (q->tail == node) {
        q->tail = node->prev;
        q->tail->next = NULL;
        free(node);
        return data;
    }

    node->prev->next = node->next;
    node->next->prev = node->prev;

    free(node);

    return data;
}

// Frees a queue and all data it holds using the provided data_free function
void queue_free(queue_t *q, void (*data_free)(void *data)) {
    list_free((list_t *)q, data_free);
}