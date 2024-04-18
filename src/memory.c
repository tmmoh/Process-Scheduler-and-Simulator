#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include "memory.h"
#include "process.h"


// End of Definitions ------------------------------------------------------------
// -------------------------------------------------------------------------------


// Initialises a memory strucuture of the given type
// To be used by a process scheduler
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

// Tries to allocate memory to given process
// Returns 0 if unsuccessful, the amount of memory allocated otherwise
int mem_alloc(mem_t* mem, process_t* p) {
    switch (mem->type) {
        case INFINITE:
            return 0;
        case FIRST_FIT:
            return first_fit(mem->data, p);
        case PAGED:
            return page_fit(mem->data, p);
        case VIRTUAL:
            return 0;
    }

    return 0;
}

// Frees the memory used by a provided process
void mem_free(mem_t* mem, process_t* p) {
    switch (mem->type) {
        case INFINITE:
            break;
        case FIRST_FIT:
            return free_block(mem->data, p);
        case PAGED:
            return evict_pages(mem->data, p);
        case VIRTUAL:
            break;
    }
}

// Frees the memory structure and the internally used memory type struct
void mem_struct_free(mem_t* mem) {
    switch (mem->type) {
        case INFINITE:
            break;
        case FIRST_FIT:
            list_free(mem->data, free);
            break;
        case PAGED:
            free(((paged_mem_t*) mem->data)->frames);
            free(mem->data);
            break;
        case VIRTUAL:
            break;
    }

    free(mem);
}

// Returns the percentage of used memory for the provided mem struct
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


// Contiguous memory method implementations --------------------------------------
// -------------------------------------------------------------------------------

// Initialises unallocated contiguous memory of size MAX_MEM
cont_mem_t* cont_mem_init() {
    cont_mem_t* mem = (cont_mem_t*) new_list();
    mem_block_t* block = malloc(sizeof(*block));
    assert(block);

    block->allocated = 0;
    block->start = 0;
    block->end = MAX_MEM - 1;
    block->size = MAX_MEM;

    list_add_tail((list_t*) mem, block);

    return mem;
}

// Tries to allocate memory to given process using a first-fit policy
// Returns 0 if unsuccessful, the amount of memory allocated otherwise
int first_fit(cont_mem_t* mem, process_t* p) {
    node_t* curr = mem->head;

    // Traverse all contiguous memory until a big enough block is found
    while (curr) {

        mem_block_t* block = (mem_block_t*) curr->data;

        if (!block->allocated && block->size >= p->mem_size) {
            // First big enough block found, allocate memory
            if (p->mem_size == block->size) {
                // Allocate the whole block
                block->allocated = 1;
                p->mem = curr;
                return block->size;
            }

            // Only part of the block needs to be allocated, fragment block

            // Create the new node in the linked list and place it before current node
            node_t* new = malloc(sizeof(*new));
            assert(new);
            mem->len += 1;

            if (!curr->prev) {
                // Curr is head
                mem->head = new;
                new->next = curr;
                curr->prev = new;
                new->prev = NULL;
            } else {
                new->prev = curr->prev;
                new->next = curr;
                curr->prev = new;
                new->prev->next = new;
            }

            // Initialise the data of the new allocated block
            mem_block_t* new_block = malloc(sizeof(*new_block));
            assert(new_block);

            new_block->allocated = 1;
            new_block->start = block->start;
            new_block->end = block->start + p->mem_size - 1;
            new_block->size = p->mem_size;

            new->data = new_block;
            p->mem = new;

            // Update the free block, making it smaller
            block->start = new_block->start + p->mem_size;
            block->size = block->end - block->start + 1;

            return p->mem_size;
        }

        curr = curr->next;
    }
    
    return 0;
}

// Frees a block memory used by a provided process
void free_block(cont_mem_t* mem, process_t* process) {
    node_t* block = process->mem;
    process->mem = NULL;

    // No allocated memory to process, return
    if (!block) return;

    mem_block_t* b = (mem_block_t*) block->data;
    mem_block_t* p;
    mem_block_t* n;

    node_t* prev = block->prev;
    node_t* next = block->next;

    // Both prev and next are null, the block is the entire memory
    if (!prev && !next) {
        // Simply freeing it is sufficient
        b->allocated = 0;       
        return;
    }

    // Only prev is null
    if (!prev) {
        n = (mem_block_t*) next->data;
        // Two cases, no action needed besides freeing if next is allocated
        if (n->allocated) {
            b->allocated = 0;
        } else {
            // Merge the mem blocks
            mem->len -= 1;
            b->allocated = 0;
            block->next = next->next;
            if (block->next) block->next->prev = block;
            b->end = n->end;
            b->size = b->end - b->start + 1;
            free(n);
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
            mem->len -= 1;
            prev->next = NULL;
            mem->tail = prev;
            p->end = b->end;
            p->size = p->end - p->start + 1;
            free(b);
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
        mem->len -= 1;
        b->allocated = 0;
        block->next = next->next;
        if (block->next) block->next->prev = block;
        b->end = n->end;
        b->size = b->end - b->start + 1;
        free(n);
        free(next);
        return;
    }

    if (!p->allocated && n->allocated) {
        // Merge block with prev
        mem->len -= 1;
        prev->next = block->next;
        if (prev->next) prev->next->prev = prev;
        p->end = b->end;
        p->size = p->end - p->start + 1;
        free(b);
        free(block);
        return;
    }

    if (!p->allocated && !n->allocated) {
        // Merge all three
        mem->len -= 2;
        prev->next = next->next;
        if (prev->next) prev->next->prev = prev;
        if (mem->tail == next) mem->tail = prev;
        p->end = n->end;
        p->size = p->end - p->start + 1;
        free(b);
        free(block);
        free(n);
        free(next);
        return;
    }
}


// Paged memory method implementations -------------------------------------------
// -------------------------------------------------------------------------------

// Initialises unallocated paged memory of size MAX_MEM and frame size FRAME_SIZE
paged_mem_t* paged_mem_init() {
    paged_mem_t* mem = malloc(sizeof(*mem));
    assert(mem);

    mem->allocatable = MAX_MEM;
    mem->used = 0;
    mem->n_frames = MAX_MEM / FRAME_SIZE;
    mem->frames = calloc(mem->n_frames, sizeof(int));

    return mem;
}

// Initialises a page table that covers a provided size of memory
page_table_t* page_table_init(size_t mem_size) {
    page_table_t* table = malloc(sizeof(*table));
    assert(table);

    table->n_pages = ceil(mem_size / (double) FRAME_SIZE);
    table->pages = malloc(sizeof(*table->pages) * table->n_pages);
    assert(table->pages);
    
    return table;
}

// Tries to allocate memorty to a given process using paged memory
// Returns 0 if unsuccessful, the amount of memory allocated otherwise
int page_fit(paged_mem_t* mem, process_t* p) {
    if (p->mem_size > mem->allocatable) {
        // Cannot allocate any memory
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

    return ceil(p->mem_size / (double) FRAME_SIZE);
}

// Evicts the pages used by a provided process
void evict_pages(paged_mem_t* mem, process_t* p) {
    if (!p->mem) return;

    page_table_t* table = (page_table_t*) p->mem;

    // Free every frame used by the process
    for (int i = 0; i < table->n_pages; i++) {
        mem->frames[table->pages[i]] = 0;
    }

    mem->allocatable += table->n_pages * FRAME_SIZE;
    mem->used -= p->mem_size;

    free(table->pages);
    free(table);
    p->mem = NULL;
}