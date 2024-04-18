#ifndef _CONFIG_H_
#define _CONFIG_H_

// Enum definining the different options for memory management
typedef enum mem_opt {
    INFINITE,
    FIRST_FIT,
    PAGED,
    VIRTUAL,
} mem_opt_t;

// Struct to hold all command line arguments and run options
typedef struct run_options {
    char *filename;
    mem_opt_t mem;
    int quantum;
} run_opts_t;

#endif