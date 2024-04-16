#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

#include "roundrobin.h"
#include "queue.h"
#include "process.h"

rr_t* new_rr(int quantum) {
    rr_t* rr = malloc(sizeof(*rr));
    assert(rr);

    rr->quantum = quantum;
    rr->time = 0;
    rr->running = NULL;
    rr->ready = new_queue();
    rr->processes = new_queue();

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
    if (rr->running && rr->running->service <= 0) {
        rr_finish_process(rr);
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
        rr->running->service -= rr->quantum;
    }
}

void rr_finish_process(rr_t* rr) {
    printf("%d,FINISHED,process-name=%s,proc-remaining=%ld\n", rr->time, rr->running->name, rr->ready->len);

    free(rr->running);
    rr->running = NULL;
}


void rr_start_next(rr_t* rr) {
    rr->running = dequeue(rr->ready);
    printf("%d,RUNNING,process-name=%s,remaining-time=%d\n", rr->time, rr->running->name, rr->running->service);
}