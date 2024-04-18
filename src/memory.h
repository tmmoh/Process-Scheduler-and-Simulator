#ifndef _MEMORY_H_
#define _MEMORY_H_

#include <stdlib.h>
#include "linkedlist.h"
#include "process.h"
#include "config.h"

#define MAX_MEM 2048
#define FRAME_SIZE 4
#define MIN_PAGES 4

// Visible interface for memory management

// Struct defines the memory management type and has an internal pointer to
// the physical implementation of the memory management type
typedef struct mem {
    mem_opt_t type;
    void* data;
} mem_t;

// Initialises a memory strucuture of the given type
// To be used by a process scheduler
mem_t* mem_init(mem_opt_t type);

// Tries to allocate memory to given process
// Returns 0 if unsuccessful, the amount of memory allocated otherwise
int mem_alloc(mem_t* mem, process_t* p);

// Frees the memory used by a provided process
void mem_free(mem_t* mem, process_t* p);

// Frees the memory structure and the internally used memory type struct
void mem_struct_free(mem_t* mem);

// Returns the percentage of used memory for the provided mem struct
int mem_usage(mem_t* mem);

// Checks whether a process has enough memory to run
// Returns 1 if the process can run, 0 otherwise
int mem_check(mem_t* mem, process_t* p);

// Contiguous memory structures and methods definitions --------------------------
// -------------------------------------------------------------------------------
typedef struct mem_block mem_block_t;
struct mem_block {
    int allocated;
    int start;
    int end;
    int size;
};

// Contiguous memory is a wrapper struct over a doubly-linked list
typedef list_t cont_mem_t;

// Initialises unallocated contiguous memory of size MAX_MEM
cont_mem_t* cont_mem_init();

// Tries to allocate memory to given process using a first-fit policy
// Returns 0 if unsuccessful, the amount of memory allocated otherwise
int first_fit(cont_mem_t* mem, process_t* p); 

// Frees a block memory used by a provided process
void free_block(cont_mem_t* mem, process_t* p);


// Paged memory structures and methods definitions -------------------------------
// -------------------------------------------------------------------------------

typedef struct paged_mem {
    size_t allocatable;
    size_t used;
    int* frames;
    int n_frames;
} paged_mem_t;

typedef struct page_table {
    int* pages;
    int n_pages;
    int allocated;
} page_table_t;

// Initialises unallocated paged memory of size MAX_MEM and frame size FRAME_SIZE
paged_mem_t* paged_mem_init();

// Initialises a page table that covers a provided size of memory
page_table_t* page_table_init(size_t mem_size);

// Tries to allocate memorty to a given process using paged memory
// Returns 0 if unsuccessful, the amount of memory allocated otherwise
int fit_pages(paged_mem_t* mem, process_t* p);

// Evicts the pages used by a provided process
void evict_all_pages(paged_mem_t* mem, process_t* p);

// Tries to allocate as many pages as possible to the process
// Returns 0 if unsuccessful, the number of pages allocated otherwise;
int allocate_pages(paged_mem_t* mem, process_t* p);

// Evicts enough pages of a process for another process to run
void evict_pages(paged_mem_t* mem, process_t* p);

#endif