#ifndef _CONFIG_H_
#define _CONFIG_H_

typedef enum mem_opt {
    INFINITE,
    FIRST_FIT,
    PAGED,
    VIRTUAL,
} mem_opt_t;

typedef struct run_options {
    char* filename;
    mem_opt_t mem;
    int quantum;
} run_opts_t;


#endif