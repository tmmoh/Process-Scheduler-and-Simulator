#ifndef _PROCESS_H_
#define _PROCESS_H_

typedef struct process process_t;
#include "memory.h"

#define NAME_MAX_LENGTH 8
#define MAX_PROCESS_LINE_LENGTH (10 + 8 + 10 + 4 + 4)

struct process {
    size_t arrived;
    char* name;
    size_t service;
    long long remaining;
    int mem_size;
    node_t* mem;
};

process_t* parse_process(char* line);

void process_free(process_t* p);

#endif