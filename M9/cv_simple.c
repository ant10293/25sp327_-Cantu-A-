#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include "common.h"
#include "queue.h"
#include "request.h"   

#define NUM_ITEMS 100

static queue_t *q;


static void *producer(void *arg)
{
    (void)arg;
    for (int i = 0; i < NUM_ITEMS; ++i) {
        int *val = malloc(sizeof *val);
        *val = i;
        request_t *r = malloc(sizeof *r);
        r->arg = val;
        r->work_fn = NULL;      /* unused in this demo */
        queue_enqueue(q, r);
    }
    queue_close(q);             /* signal no more data */
    return NULL;
}


static void *consumer(void *arg)
{
    (void)arg;
    long long sum = 0;
    for (;;) {
        request_t *r = (request_t *)queue_dequeue(q);
        if (!r) break;          /* queue closed & empty */
        sum += *((int *)r->arg);
        free(r->arg);
        free(r);
    }
    printf("consumer: computed sum = %lld (expect %d)\n",
           sum, (NUM_ITEMS - 1) * NUM_ITEMS / 2);
    return NULL;
}


int main(void)
{
    pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
    pthread_cond_t  cv  = PTHREAD_COND_INITIALIZER;

    q = queue_init(&mtx, &cv);

    pthread_t prod, cons;
    pthread_create(&prod, NULL, producer, NULL);
    pthread_create(&cons, NULL, consumer, NULL);

    pthread_join(prod, NULL);
    pthread_join(cons, NULL);

    queue_destroy(q);
    pthread_mutex_destroy(&mtx);
    pthread_cond_destroy(&cv);
    return 0;
}
