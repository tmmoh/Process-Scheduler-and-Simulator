#ifndef _ROUND_ROBIN_H_
#define _ROUND_ROBIN_H_

#include "process.h"
#include "queue.h"
#include "config.h"
#include "memory.h"

typedef struct rr_scheduler {
    run_opts_t* opts;
    size_t time;
    process_t* running;
    queue_t* ready;
    queue_t* processes;
    queue_t* lru;
    mem_t* mem;
    size_t process_count;
    double total_turnaround;
    double total_overhead;
    double max_overhead;
} rr_t;

rr_t* new_rr(run_opts_t* opts);

void rr_add_process(rr_t* rr, process_t* process);

void rr_ready_process(rr_t* rr, process_t* process);

void rr_finish_process(rr_t* rr);

void rr_simulate(rr_t* rr);

void rr_simulate_cycle(rr_t* rr);

void rr_start_next(rr_t* rr);

void rr_free(rr_t* rr);

#endif