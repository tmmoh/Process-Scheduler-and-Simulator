#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include "memory.h"
#include "process.h"



cont_mem_t* cont_mem_init() {
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

void free_pages(paged_mem_t* mem, process_t* p) {
    page_table_t* table = (page_table_t*) p->mem;
    for (int i = 0; i < table->n_pages; i++) {
        mem->frames[table->pages[i]] = 0;
    }
    mem->allocatable += table->n_pages * FRAME_SIZE;
    mem->used -= p->mem_size;
}

page_table_t* page_table_init(size_t mem_size) {
    page_table_t* table = malloc(sizeof(*table));
    assert(table);

    table->n_pages = ceil((long long) mem_size / (double) FRAME_SIZE);
    table->pages = malloc(sizeof(*table->pages) * table->n_pages);
    assert(table->pages);
    
    return table;
}

int page_fit(paged_mem_t* mem, process_t* p) {
    if (p->mem_size > mem->allocatable) {
        return 0;
    }

    page_table_t* table = page_table_init(p->mem_size);

    int frame = 0;
    for (int i = 0; i < table->n_pages; i++) {
       // Should never go past max index since memory is allocatable
       // Find the next unallocated frame
       while (mem->frames[frame]) frame++;

       // Allocate frame
       table->pages[i] = frame;
       mem->frames[frame] = 1;
       mem->allocatable -= FRAME_SIZE;
    }

    mem->used += p->mem_size;

    p->mem = table;

    return 1;
}

paged_mem_t* paged_mem_init() {
    paged_mem_t* mem = malloc(sizeof(*mem));
    assert(mem);

    mem->allocatable = MAX_MEM;
    mem->n_frames = MAX_MEM / FRAME_SIZE;
    mem->frames = calloc(mem->n_frames, sizeof(int));

    return mem;
}

int mem_usage(mem_t* mem) {
    
    switch (mem->type) {
        case INFINITE:
            return 0;
        case FIRST_FIT:
            node_t* curr = ((cont_mem_t*) mem->data)->head;
            double used = 0;
            double total = 0;

            while (curr) {
                mem_block_t* block = (mem_block_t*) curr->data;
                if (block->allocated) used += block->size;
                total += block->size;

                curr = curr->next;
            }

            return ceil(used/total * 100);
        case PAGED:
            return ceil(((paged_mem_t*) mem->data)->used / (double)MAX_MEM * 100.0); 
        case VIRTUAL:
    }

    return 0;
}

mem_t* mem_init(mem_opt_t type) {
    mem_t* mem = malloc(sizeof(*mem));
    assert(mem);

    mem->type = type;
    mem->data = NULL;

    switch (mem->type) {
        case INFINITE:
            break;
        case FIRST_FIT:
            mem->data = cont_mem_init();
            break;
        case PAGED:
            mem->data = paged_mem_init();
            break;
        case VIRTUAL:
    }

    return mem;
}

int mem_alloc(mem_t* mem, process_t* p) {
    switch (mem->type) {
        case INFINITE:
        case FIRST_FIT:
            return first_fit(mem->data, p);
        case PAGED:
            return page_fit(mem->data, p);
        case VIRTUAL:
    }

    return 0;
}

void mem_free(mem_t* mem, process_t* p) {
    switch (mem->type) {
        case INFINITE:
        case FIRST_FIT:
            return free_block((node_t*) p->mem);
        case PAGED:
            return free_pages(mem->data, p);
        case VIRTUAL:
    }
}