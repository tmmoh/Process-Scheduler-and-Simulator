#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <math.h>

#include "roundrobin.h"
#include "queue.h"
#include "process.h"

#define TWO_DP(x) (round(x * 100.0) / 100.0)

rr_t* new_rr(int quantum) {
    rr_t* rr = malloc(sizeof(*rr));
    assert(rr);

    rr->quantum = quantum;
    rr->time = 0;
    rr->running = NULL;
    rr->ready = new_queue();
    rr->processes = new_queue();
    rr->process_count = 0;
    rr->total_turnaround = 0;
    rr->total_overhead = 0;
    rr->max_overhead = 0;

    return rr;
}

void rr_add_process(rr_t* rr, process_t* process) {
    enqueue(rr->processes, process);
}

void rr_ready_process(rr_t* rr, process_t* process) {
    enqueue(rr->ready, process);
}

void rr_simulate(rr_t* rr) {
    while (rr->running || rr->processes->len > 0) {
        rr_simulate_cycle(rr);
    }

    printf("Turnaround time %.0f\n", round(rr->total_turnaround / rr->process_count));
    printf("Time overhead %.2f %.2f\n", TWO_DP(rr->max_overhead), TWO_DP(rr->total_overhead / rr->process_count));
    printf("Makespan %ld\n", rr->time);
}

void rr_simulate_cycle(rr_t* rr) {
    // Check if a new process is ready to be added
    int arrival;
    do {
        if (rr->processes->len < 1) break;
        process_t* next = (process_t*) rr->processes->head->data;

        arrival = next->arrived;
        if (arrival <= rr->time) {
            rr_ready_process(rr, (process_t*) dequeue(rr->processes));
        }
    } while (arrival <= rr->time);

    // Check finished process
    if (rr->running && rr->running->remaining <= 0) {
        rr_finish_process(rr);
        if (rr->ready->len < 1 && rr->processes->len < 1) return;
    }

    // Check if there's waiting ready processes
    if (rr->ready->len > 0) {
        // Ready the running process
        if (rr->running) {
            rr_ready_process(rr, rr->running);
            rr->running = NULL;
        }

        // Run the next ready process
        rr_start_next(rr);
    }

    // Run for a quantum
    rr->time += rr->quantum;
    if (rr->running) {
        rr->running->remaining -= rr->quantum;
    }
}

void rr_finish_process(rr_t* rr) {
    printf("%ld,FINISHED,process-name=%s,proc-remaining=%ld\n", rr->time, rr->running->name, rr->ready->len);

    int turnaround = rr->time - rr->running->arrived;
    rr->process_count += 1;
    rr->total_turnaround += turnaround;
    
    double overhead = (double) turnaround / (double) rr->running->service;
    rr->total_overhead += overhead;
    rr->max_overhead = overhead > rr->max_overhead ? overhead : rr->max_overhead;

    free(rr->running);
    rr->running = NULL;
}


void rr_start_next(rr_t* rr) {
    rr->running = dequeue(rr->ready);
    printf("%ld,RUNNING,process-name=%s,remaining-time=%lld\n", rr->time, rr->running->name, rr->running->remaining);
}