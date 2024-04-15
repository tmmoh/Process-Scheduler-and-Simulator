#ifndef _PROCESS_H_
#define _PROCESS_H_

#define NAME_MAX_LENGTH 8
#define MAX_PROCESS_LINE_LENGTH (10 + 8 + 10 + 4 + 4)

typedef struct process {
    int arrived;
    char* name;
    int service;
    int mem;
} process_t;

process_t* parse_process(char* line);

#endif