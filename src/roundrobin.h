#ifndef _ROUND_ROBIN_H_
#define _ROUND_ROBIN_H_

#include "config.h"
#include "memory.h"
#include "process.h"
#include "queue.h"

// Struct to hold round-robin scheduler's required information
typedef struct rr_scheduler {
    run_opts_t *opts;
    unsigned long time;
    process_t *running;
    queue_t *ready;
    queue_t *processes;
    queue_t *lru;
    mem_t *mem;
    size_t process_count;
    long double avg_turnaround;
    long double total_overhead;
    long double max_overhead;
} rr_t;

// Creates and returns a new round-robing scheduler
// Configures the scheduler according the run_options
rr_t *new_rr(run_opts_t *opts);

// Add's a process to round-scheduler
// The process will be readied and run when it "arrives"
void rr_add_process(rr_t *rr, process_t *process);

// Simulates the entirety of the configured scheduler
void rr_simulate(rr_t *rr);

#endif