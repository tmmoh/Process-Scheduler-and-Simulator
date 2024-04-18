#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <math.h>

#include "roundrobin.h"

#define TWO_DP(x) (round(x * 100.0) / 100.0)
#define MIN(a, b) (a < b ? a : b)

#pragma region Round Robin Scheduler Method Definitions

// Simulates only one cycle of the configured scheduler
void rr_simulate_cycle(rr_t* rr);

// Starts the next process in the scheduler's ready queue
void rr_start_next(rr_t* rr);

// Places a process in the scheduler's ready queue
void rr_ready_process(rr_t* rr, process_t* process);

// Fishes a process's execution and prints scheduler statistics
void rr_finish_process(rr_t* rr);

// Evicts all pages used by the least recently used process
void rr_evict_lru(rr_t* rr);

// Frees the scheduler and all associated data structures
void rr_free(rr_t* rr);


#pragma endregion

#pragma region Round Robin Scheduler Method Implemenations

// Creates and returns a new round-robing scheduler
// Configures the scheduler according the run_options
rr_t* new_rr(run_opts_t* opts) {
    rr_t* rr = malloc(sizeof(*rr));
    assert(rr);

    rr->opts = opts;
    rr->time = 0;
    rr->running = NULL;
    rr->ready = new_queue();
    rr->processes = new_queue();
    rr->process_count = 0;
    rr->total_turnaround = 0;
    rr->total_overhead = 0;
    rr->max_overhead = 0;
    rr->lru = new_queue();

    rr->mem = mem_init(opts->mem);

    return rr;
}

// Add's a process to round-scheduler
// The process will be readied and run when it "arrives"
void rr_add_process(rr_t* rr, process_t* process) {
    enqueue(rr->processes, process);
}

// Simulates the entirety of the configured scheduler
void rr_simulate(rr_t* rr) {
    while (rr->running || rr->processes->len > 0) {
        rr_simulate_cycle(rr);
    }

    printf("Turnaround time %.0f\n", round(rr->total_turnaround / (long double)rr->process_count));
    printf("Time overhead %.2f %.2f\n", TWO_DP(rr->max_overhead), TWO_DP(rr->total_overhead / rr->process_count));
    printf("Makespan %lld\n", rr->time);
    rr_free(rr);
}

// Simulates only one cycle of the configured scheduler
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
        if (rr->running) rr_ready_process(rr, rr->running);


        switch(rr->opts->mem) {
            case INFINITE:
                // Run the next ready process
                rr_start_next(rr);
                break;
            case FIRST_FIT:
                // Find the next process with allocated memory or can allocate memory
                while (!((process_t*) rr->ready->head->data)->mem && !mem_alloc(rr->mem, rr->ready->head->data)) {
                    requeue_head(rr->ready);
                }
                rr_start_next(rr);
                break;
            case PAGED:
                while (!((process_t*) rr->ready->head->data)->mem && !mem_alloc(rr->mem, rr->ready->head->data)) {
                    rr_evict_lru(rr);
                }
                rr_start_next(rr);
                break;
            case VIRTUAL:
                break;
        }
    }


    // Run for a quantum
    rr->time += rr->opts->quantum;
    if (rr->running) {
        rr->running->remaining -= MIN(rr->running->remaining, rr->opts->quantum);
    }
}

// Places a process in the scheduler's ready queue
void rr_ready_process(rr_t* rr, process_t* process) {
    enqueue(rr->ready, process);
}

// Fishes a process's execution and prints scheduler statistics
void rr_finish_process(rr_t* rr) {

    node_t* curr = rr->lru->head;
    while(curr) {
        if (rr->running == curr->data) {
            queue_remove(rr->lru, curr);
            break;
        }
        curr = curr->next;
    }
    

    if (rr->opts->mem == PAGED) {
        printf("%lld,EVICTED,evicted-frames=[", rr->time);
        for (int i = 0; i < ((page_table_t*) rr->running->mem)->n_pages; i++) {
            if (i != 0) printf(",");
            printf("%d", ((page_table_t*) rr->running->mem)->pages[i]);
        }
        printf("]\n");
    }

    mem_free(rr->mem, rr->running);
    
    printf("%lld,FINISHED,process-name=%s,proc-remaining=%ld\n", rr->time, rr->running->name, rr->ready->len);

    int turnaround = rr->time - rr->running->arrived;
    rr->process_count += 1;
    rr->total_turnaround += turnaround;
    
    double overhead = (double) turnaround / (double) rr->running->service;
    rr->total_overhead += overhead;
    rr->max_overhead = overhead > rr->max_overhead ? overhead : rr->max_overhead;

    process_free(rr->running);
    rr->running = NULL;
}

// Starts the next process in the scheduler's ready queue
void rr_start_next(rr_t* rr) {
    rr->running = dequeue(rr->ready);
    
    // Navigate through lru queue, find process and requeue
    // Or add process to lru queue if run for first time
    node_t* curr = rr->lru->head;
    int found = 0;
    while(curr) {
        if (curr->data == rr->running) {
            requeue(rr->lru, curr);
            found = 1;
            break;
        }
        curr = curr->next;
    }
    if (!found) enqueue(rr->lru, rr->running);

    printf("%lld,RUNNING,process-name=%s,remaining-time=%ld", rr->time, rr->running->name, rr->running->remaining);

    switch (rr->opts->mem) {
        case FIRST_FIT:
            printf(",mem-usage=%d%%,allocated-at=%d", mem_usage(rr->mem), ((mem_block_t*) ((node_t*) rr->running->mem)->data)->start);
            break;
        case PAGED:
            printf(",mem-usage=%d%%,mem-frames=[", mem_usage(rr->mem));
            for (int i = 0; i < ((page_table_t*) rr->running->mem)->n_pages; i++) {
                if (i != 0) printf(",");
                printf("%d", ((page_table_t*) rr->running->mem)->pages[i]);
            }
            printf("]");
            break;
        case VIRTUAL:
            break;
        case INFINITE:
            break;
    }

    printf("\n");
}

// Evicts all pages used by the least recently used process
void rr_evict_lru(rr_t* rr) {
    process_t* lru = dequeue(rr->lru);

    printf("%lld,EVICTED,evicted-frames=[", rr->time);
    for (int i = 0; i < ((page_table_t*) lru->mem)->n_pages; i++) {
        if (i != 0) printf(",");
        printf("%d", ((page_table_t*) lru->mem)->pages[i]);
    }
    printf("]\n");

    mem_free(rr->mem, lru);
    lru->mem = NULL;
}

// Frees the scheduler and all associated data structures
void rr_free(rr_t* rr) {
    queue_free(rr->ready, (void*) process_free);
    queue_free(rr->processes, (void*) process_free);
    free(rr->lru);
    mem_struct_free(rr->mem);
    free(rr);
}

#pragma endregion