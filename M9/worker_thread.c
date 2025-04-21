#include "worker_thread.h"
#include "request.h"     
#include <string.h>
#include "common.h"


void* do_work(void* arg)
{
    struct worker_thread_params* p = (struct worker_thread_params*)arg;
    fprintf(stderr, "thread-%d starting\n", p->thread_id);

    int processed = 0;
    for (;;) {
        request_t* req = queue_dequeue(p->que);
        if (!req) break;                         
        req->work_fn(req->arg);                
        free(req);                               
        processed++;
    }

    fprintf(stderr, "thread-%d exiting. Processed %d requests.\n",
            p->thread_id, processed);
    free(p);                                   
    pthread_exit(NULL);
}
