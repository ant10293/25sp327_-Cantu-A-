#include "thread_pool.h"
#include "common.h"

static pthread_t create_worker_thread(struct worker_thread_params* p)
{
    pthread_t tid;
    int rc = pthread_create(&tid, NULL, do_work, p);
    if (rc != 0) handle_error_en(rc, "pthread_create");
    return tid;
}

struct thread_pool* thread_pool_init(queue_t* que, int nthreads)
{
    struct thread_pool* pool = malloc(sizeof(struct thread_pool));
    if (!pool) handle_error("thread_pool_init:malloc");

    pool->next_thread_id = 1;
    pool->num_threads    = nthreads;
    pool->que            = que;

    for (int i = 0; i < nthreads; ++i) {
        struct worker_thread_params* p = malloc(sizeof *p);
        p->que = que;
        p->thread_id = pool->next_thread_id;

        pool->threads[i] = malloc(sizeof(struct worker_thread));
        pool->threads[i]->thread_id = pool->next_thread_id;
        pool->threads[i]->thread    = create_worker_thread(p);

        pool->next_thread_id++;
    }
    return pool;
}

void thread_pool_destroy(struct thread_pool* pool)
{
    for (int i = 0; i < pool->num_threads; ++i)
        pthread_join(pool->threads[i]->thread, NULL);

    for (int i = 0; i < pool->num_threads; ++i)
        free(pool->threads[i]);

    free(pool);
}
