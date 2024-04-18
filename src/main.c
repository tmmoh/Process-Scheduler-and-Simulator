#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "config.h"
#include "process.h"
#include "roundrobin.h"

#define OPT_STRING ":f:m:q:"

// Argument buffer for getopts()
extern char *optarg;

// Parses run options from the command line
// Returns a run_options struct with the arguments
run_opts_t *parse_options(int argc, char **argv);

// Prints an error message to stderr and exits the process with an error
void parse_fail(char *process_name);

int main(int argc, char **argv) {

    run_opts_t *run_opts = parse_options(argc, argv);

    FILE *input = fopen(run_opts->filename, "r");
    assert(input);

    char buffer[MAX_PROCESS_LINE_LENGTH + 1];

    // Create the round robin scheduler with the right configurations
    rr_t *rr = new_rr(run_opts);

    // Read all the processes from file and add them to the scheduler
    // Assumes processes are in arrival time order in file
    while (fgets(buffer, MAX_PROCESS_LINE_LENGTH, input) != NULL) {
        process_t *process = parse_process(buffer);
        rr_add_process(rr, process);
    }

    // Simulate the round robin scheduler
    rr_simulate(rr);
    rr = NULL;

    // Free all used data structures
    free(run_opts);
    fclose(input);

    exit(EXIT_SUCCESS);
}

// Parses run options from the command line
// Returns a run_options struct with the arguments
run_opts_t *parse_options(int argc, char **argv) {
    run_opts_t *opts = malloc(sizeof(*opts));
    assert(opts);

    char *optstring = OPT_STRING;
    int c;

    int f_flag = 0, m_flag = 0, q_flag = 0;

    // Parse command line arguments
    while ((c = getopt(argc, argv, optstring)) != -1) {

        switch (c) {

        case 'f': // Read the filename used
            f_flag++;
            opts->filename = optarg;
            break;

        case 'm': // Read the memory management type
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
                // Not a support memory option
                parse_fail(argv[0]);
            }
            break;

        case 'q': // Read the quantum option
            q_flag++;

            int quantum = atoi(optarg);
            if (quantum < 1 || quantum > 3) {
                // Not a supported memory option
                parse_fail(argv[0]);
            }

            opts->quantum = quantum;
            break;

        default: // Not a supported flag
            parse_fail(argv[0]);
        }
    }

    // Check for a missing flag or too many flags
    if (f_flag != 1 || m_flag != 1 || q_flag != 1) {
        parse_fail(argv[0]);
    }

    return opts;
}

// Prints an error message to stderr and exits the process with an error
void parse_fail(char *process_name) {
    fprintf(stderr,
            "Usage: %s -f <filename> -m (infinite | first-fit | paged | "
            "virtual) -q (1 | 2 | 3)\n",
            process_name);
    exit(EXIT_FAILURE);
}
