#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "process.h"

#define DELIM " "

// Parses and creates a process from a process line
process_t *parse_process(char *line) {
    char *delim = DELIM;
    char *tok;

    process_t *process = malloc(sizeof(*process));
    assert(process);

    // Parse process arrival time
    tok = strtok(line, delim);
    process->arrived = atoi(tok);

    // Parse process name
    tok = strtok(NULL, delim);
    process->name = strdup(tok);

    // Parse process service time and remaining time
    tok = strtok(NULL, delim);
    process->service = atoi(tok);
    process->remaining = process->service;

    // Parse process required memory
    tok = strtok(NULL, delim);
    process->mem_size = atoi(tok);

    // No memory to point to yet
    process->mem = NULL;

    return process;
}

// Frees a process and all associated fields
void process_free(process_t *p) {
    free(p->name);
    free(p);
}
