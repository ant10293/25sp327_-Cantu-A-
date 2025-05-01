#ifndef WORKER_THREAD_POOL_H
#define WORKER_THREAD_POOL_H

#include <pthread.h>
#include <stdio.h>
#include "queue.h"
#include "worker_thread.h"

#define MAX_WORKER_THREADS 20

/// @brief Worker thread node stored in worker_thread_pool's
struct worker_thread {
    int thread_id;     // thread id - a sequentially assigned number
    pthread_t thread;  // thread identifier returned by pthread_create
};

/// @brief The worker thread pool
struct thread_pool {
    int next_thread_id;                                // next thread id to assign
    int num_threads;                                   // number of threads in pool
    struct worker_thread* threads[MAX_WORKER_THREADS];  // threads
    queue_t* que;                                      // queue used by threads
};

typedef struct thread_pool thread_pool_t;

/// @brief Initializes the thread pool with num_threads.
/// @param que the queue used by worker threads to dequeue requests
/// @param num_threads the number of worker threads to put in the pool
extern struct thread_pool* thread_pool_init(queue_t* que, int num_threads);

/// @brief Waits for all worker threads to exit and performs clean up work.
/// This function should be called after the queue is closed. 
/// @param pool the thread pool
extern void thread_pool_destroy(struct thread_pool* pool);

#endif