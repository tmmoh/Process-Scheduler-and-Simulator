#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "process.h"

process_t* parse_process(char* line) {
    char* delim = " ";
    char* tok;

    process_t* process = malloc(sizeof(*process));
    assert(process);

    tok = strtok(line, delim);
    process->arrived = atoi(tok);

    tok = strtok(NULL, delim);
    process->name = strdup(tok);

    tok = strtok(NULL, delim);
    process->service = atoi(tok);
    process->remaining = process->service;

    tok = strtok(NULL, delim);
    process->mem_size = atoi(tok);

    process->mem = NULL;

    return process;
}

void process_free(process_t* p) {
    free(p->name);
    free(p);
}
