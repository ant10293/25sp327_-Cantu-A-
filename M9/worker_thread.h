#ifndef WORKER_THREAD_H
#define WORKER_THREAD_H

#include <pthread.h>
#include <stdio.h>
#include "queue.h"

/// @brief The parameters passed to the worker thread.
struct worker_thread_params {
    int thread_id;  // a sequential number to identify the thread
    queue_t* que;   // the queue to pull requests from
};

/// @brief This is the function to pass to pthread_create(3). The
/// thread_params should be cast to a worker_thread_param. This
/// function dequeues requests and performs the designated work.
/// @param thread_params
/// @return NULL
extern void* do_work(void* thread_params);

#endif
