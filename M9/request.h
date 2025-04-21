#ifndef REQUEST_H
#define REQUEST_H

typedef struct request {
    void (*work_fn)(void*);  
    void* arg;               
} request_t;

#endif
