#ifndef _MEMORY_H_
#define _MEMORY_H_

#include <stdlib.h>
#include "linkedlist.h"
#include "process.h"
#include "config.h"

#define MAX_MEM 2048
#define FRAME_SIZE 4


typedef struct mem_block mem_block_t;
struct mem_block {
    int allocated;
    int start;
    int end;
    int size;
    node_t* prev;
};

typedef list_t cont_mem_t;

typedef struct page_table {
    int* pages;
    int n_pages;
} page_table_t;



typedef struct paged_mem {
    size_t allocatable;
    size_t used;
    int* frames;
    int n_frames;
} paged_mem_t;

cont_mem_t* cont_mem_init();

void free_block(node_t* block);

int first_fit(cont_mem_t* mem, process_t* p); 


typedef struct mem {
    mem_opt_t type;
    void* data;
} mem_t;

int mem_usage(mem_t* mem);

mem_t* mem_init(mem_opt_t type);

int mem_alloc(mem_t* mem, process_t* p);

void mem_free(mem_t* mem, process_t* p);


#endif