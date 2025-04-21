#include "queue_internal.h"
#include "common.h"

queue_t* queue_init(pthread_mutex_t* m, pthread_cond_t* cv)
{
    queue_t* q = malloc(sizeof(queue_t));
    if (!q) handle_error("queue_init:malloc");
    q->header = q->tail = malloc(sizeof(queue_node_t));
    if (!q->header) handle_error("queue_init:malloc header");
    q->header->next = NULL;
    q->size = 0;
    q->isclosed = false;
    q->mutex = m;
    q->cond_var = cv;
    return q;
}

static void _enqueue_node(queue_t* q, queue_node_t* n)
{
    q->tail->next = n;
    q->tail = n;
    q->size++;
}

void queue_enqueue(queue_t* q, void* data)
{
    pthread_mutex_lock(q->mutex);
    if (q->isclosed) {
        pthread_mutex_unlock(q->mutex);
        return;
    }
    queue_node_t* n = malloc(sizeof(queue_node_t));
    if (!n) handle_error("queue_enqueue:malloc node");
    n->data = data;
    n->next = NULL;
    _enqueue_node(q, n);
    pthread_cond_signal(q->cond_var);
    pthread_mutex_unlock(q->mutex);
}

void* queue_dequeue(queue_t* q)
{
    pthread_mutex_lock(q->mutex);
    while (q->header->next == NULL && !q->isclosed)
        pthread_cond_wait(q->cond_var, q->mutex);

    if (q->header->next == NULL && q->isclosed) {
        pthread_mutex_unlock(q->mutex);
        return NULL;                 
    }

    queue_node_t* n = q->header->next;
    q->header->next = n->next;
    if (q->tail == n) q->tail = q->header;
    q->size--;
    pthread_mutex_unlock(q->mutex);

    void* data = n->data;
    free(n);
    return data;
}

void queue_close(queue_t* q)
{
    pthread_mutex_lock(q->mutex);
    q->isclosed = true;
    pthread_cond_broadcast(q->cond_var);
    pthread_mutex_unlock(q->mutex);
}

int  queue_size(queue_t* q)  { return q->size; }
bool queue_isclose(queue_t* q){ return q->isclosed; }

void queue_destroy(queue_t* q)
{
    free(q->header);
    free(q);
}
