#ifndef _CONFIG_H_
#define _CONFIG_H_

typedef enum mem {
    INFINITE,
    FIRST_FIT,
    PAGED,
    VIRTUAL,
} mem_t;

typedef struct run_options {
    char* filename;
    mem_t mem;
    int quantum;
} run_opts_t;



#endif