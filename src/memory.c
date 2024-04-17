#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include "memory.h"
#include "process.h"


cont_mem_t* mem_init() {
    cont_mem_t* mem = (cont_mem_t*) new_list();
    mem_block_t* block = malloc(sizeof(*block));
    assert(block);

    block->allocated = 0;
    block->start = 0;
    block->end = MAX_MEM - 1;
    block->size = MAX_MEM;
    block->prev = NULL;

    list_add_tail((list_t*) mem, block);

    return mem;
}

void free_block(node_t* block) {
    if (!block) return;

    mem_block_t* b = (mem_block_t*) block->data;
    mem_block_t* p;
    mem_block_t* n;

    node_t* prev = b->prev;
    node_t* next = block->next;

    // Both prev and next are null, the block is the entire memory
    if (!prev && !next) {
        b->allocated = 0;       
        return;
    }

    // Only prev is null
    if (!prev) {
        n = (mem_block_t*) next->data;
        // Two cases, no action needed besides freeing if next is allocated
        b->allocated = 0;

        if (!n->allocated) {
            // Merge the mem blocks
            block->next = next->next;
            b->end = n->end;
            b->size = b->end - b->start + 1;
            free(next);
        }

        return;
    }

    // Only next is null;
    if (!next) {
        p = (mem_block_t*) prev->data;

        // Two cases, no action needed besides freeing if prev is allocated
        if (p->allocated) {
            b->allocated = 0;
        } else {
            // Merge the mem blocks
            prev->next = block->next;
            p->end = b->end;
            p->size = p->end - p->start + 1;
            free(block);
        }

        return;
    }

    n = (mem_block_t*) next->data;
    p = (mem_block_t*) prev->data;

    // Neither prev or next are null
    // There are 4 cases
    if (p->allocated && n->allocated) {
        b->allocated = 0;
        return;
    }

    if (p->allocated && !n->allocated) {
        // Merge block with next
        b->allocated = 0;
        block->next = next->next;
        b->end = n->end;
        b->size = b->end - b->start + 1;
        free(next);
        return;
    }

    if (!p->allocated && n->allocated) {
        // Merge block with prev
        prev->next = block->next;
        p->end = b->end;
        p->size = p->end - p->start + 1;
        free(block);
        return;
    }

    if (!p->allocated && !n->allocated) {
        // Merge all three
        prev->next = next->next;
        p->end = n->end;
        p->size = p->end - p->start + 1;
        free(block);
        free(next);
        return;
    }
}

int first_fit(cont_mem_t* mem, process_t* p) {
    node_t* curr = mem->head;
    while (curr) {
        mem_block_t* block = (mem_block_t*) curr->data;
        if (!block->allocated && block->size >= p->mem_size) {
            // First big enough block found, allocate memory
            if (p->mem_size == block->size) {
                // Allocate the whole block
                block->allocated = 1;
                p->mem = curr;
            } else {
                // Fragment block
                mem_block_t* new_block = malloc(sizeof(*new_block));
                assert(new_block);

                new_block->allocated = 0;
                new_block->start = block->start + p->mem_size;
                new_block->end = block->end;
                new_block->size = new_block->end - new_block->start + 1;
                new_block->prev = curr;
                
                block->allocated = 1;
                block->end = new_block->start - 1;
                block->size = p->mem_size;


                node_t* new = malloc(sizeof(*new));
                assert(new);

                new->data = new_block;

                new->next = curr->next;
                curr->next = new;

                p->mem = curr;
            }

            return 1;
        }

        curr = curr->next;
    }
    
    return 0;
}

int mem_usage(cont_mem_t* mem) {
    node_t* curr = mem->head;
    double used = 0;
    double total = 0;

    while (curr) {
        mem_block_t* block = (mem_block_t*) curr->data;
        if (block->allocated) used += block->size;
        total += block->size;

        curr = curr->next;
    }

    return ceil(used/total) * 100;
}