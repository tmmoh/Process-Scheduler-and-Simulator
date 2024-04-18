#ifndef _PROCESS_H_
#define _PROCESS_H_

typedef struct process process_t;

#include "memory.h"

// Max length of a process name 
#define NAME_MAX_LENGTH 8

// Max length of line
// 2^32 numbers are 10 digits
// Whitespace and new line char are 4 characters
// Memory is 4 digits
#define MAX_PROCESS_LINE_LENGTH (10 + NAME_MAX_LENGTH + 10 + 4 + 4)

// Struct to hold all the information relating to a process
struct process {
    size_t arrived; // Time the process arrived, int in range [0, 2^32)
    char* name; // Name of the process
    size_t service; // Service time of the process, int in range [1, 2^32)
    size_t remaining; // Remaining time for the process to complete
    int mem_size; // Size of the required memory
    void* mem; // Pointer to the allocated memory, type varies depending on memory allocation method
};

// Parses and creates a process from a process line
process_t* parse_process(char* line);

// Frees a process and all associated fields
void process_free(process_t* p);

#endif