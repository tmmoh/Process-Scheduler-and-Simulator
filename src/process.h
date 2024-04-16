#ifndef _PROCESS_H_
#define _PROCESS_H_

#define NAME_MAX_LENGTH 8
#define MAX_PROCESS_LINE_LENGTH (10 + 8 + 10 + 4 + 4)

typedef struct process {
    size_t arrived;
    char* name;
    size_t service;
    long long remaining;
    int mem;
} process_t;

process_t* parse_process(char* line);



#endif