#ifndef _ROUND_ROBIN_H_
#define _ROUND_ROBIN_H_

#include "process.h"
#include "queue.h"

typedef struct rr_scheduler {
    int quantum;
    int time;
    process_t* running;
    queue_t* ready;
    queue_t* processes;
} rr_t;

rr_t* new_rr(int quantum);

void rr_add_process(rr_t* rr, process_t* process);

void rr_ready_process(rr_t* rr, process_t* process);

void rr_finish_process(rr_t* rr);

void rr_simulate(rr_t* rr);

void rr_simulate_cycle(rr_t* rr);

void rr_start_next(rr_t* rr);


#endif