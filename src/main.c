#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>

#include "config.h"

extern char* optarg;
extern int optind, opterr, optopt;

void parse_options(int argc, char** argv, run_opts_t* opts);
void parse_fail(char* process_name);

int main(int argc, char** argv) {
    run_opts_t* run_opts = malloc(sizeof(*run_opts));
    assert(run_opts);

    parse_options(argc, argv, run_opts);

    FILE* input = fopen(run_opts->filename, "r");
    assert(input);

    exit(EXIT_SUCCESS);
}

void parse_options(int argc, char** argv, run_opts_t* opts) {
    char* optstring = ":f:m:q:";
    int c;

    int f_flag = 0, m_flag = 0, q_flag = 0;

    // Parse command line arguments
    while ((c = getopt(argc, argv, optstring)) != -1) {
        switch (c) {
            case 'f':
                f_flag++;
                opts->filename = optarg;
                break;
            case 'm':
                m_flag++;
                if (strcmp(optarg, "infinite") == 0) {
                    opts->mem = INFINITE;
                } else if (strcmp(optarg, "first-fit") == 0) {
                    opts->mem = FIRST_FIT;
                } else if (strcmp(optarg, "paged") == 0) {
                    opts->mem = PAGED;
                } else if (strcmp(optarg, "virtual") == 0) {
                    opts->mem = VIRTUAL;
                } else {
                    parse_fail(argv[0]);
                }
                break;
            case 'q':
                q_flag++;
                int quantum = atoi(optarg);
                if (quantum < 1 || quantum > 3) {
                    parse_fail(argv[0]);
                }
                opts->quantum = quantum;
                break;
            default:
                parse_fail(argv[0]);
        }
    }

    if (f_flag != 1 || m_flag != 1 || q_flag != 1) {
        parse_fail(argv[0]);
    }

}

void parse_fail(char* process_name) {
    fprintf(stderr, "Usage: %s -f <filename> -m (infinite | first-fit | paged | virtual) -q (1 | 2 | 3)\n", process_name);
    exit(EXIT_FAILURE);
}