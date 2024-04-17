#ifndef _MEMORY_H_
#define _MEMORY_H_

#include <stdlib.h>
#include "linkedlist.h"
#include "process.h"

#define MAX_MEM 2048


typedef struct mem_block mem_block_t;
struct mem_block {
    int allocated;
    int start;
    int end;
    int size;
    node_t* prev;
};

typedef list_t cont_mem_t;

cont_mem_t* mem_init();

void free_block(node_t* block);

int first_fit(cont_mem_t* mem, process_t* p); 

int mem_usage(cont_mem_t* mem);



#endif