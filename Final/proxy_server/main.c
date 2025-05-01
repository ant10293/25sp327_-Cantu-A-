#define _GNU_SOURCE

#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <pthread.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "common.h"
#include "http_proxy.h"
#include "queue.h"
#include "thread_pool.h"

#define LISTEN_PORT 8080 // Proxy listens on this port
#define BACKLOG     128
/* #define NUM_WORKERS 8 */         

// job wrapper expected by worker_thread.c                      
typedef struct job {
    void (*func)(void *); // pointer to the worker callback
    void *arg; // argument for that callback    
} job_t;

static volatile sig_atomic_t shutting_down = 0;
static void on_sigint(int _) { (void)_; shutting_down = 1; }

// create/bind/listen on LISTEN_PORT 
static int open_listener(int port) {
    int s = socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0) return -1;
    int opt = 1;
    setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in addr = {
        .sin_family      = AF_INET,
        .sin_addr.s_addr = htonl(INADDR_ANY),
        .sin_port        = htons(port)
    };
    if (bind(s, (struct sockaddr *)&addr, sizeof(addr)) < 0 ||
        listen(s, BACKLOG) < 0) {
        close(s); return -1;
    }
    return s;
}

int main(int argc, char* argv[]) {
    // Accept command-line arguments for the number of threads
    int num_threads = 2;    // provide a default number of threads
    if (argc == 2) {
        num_threads = atoi(argv[1]);
    }

    // signal handling 
    signal(SIGPIPE, SIG_IGN);
    struct sigaction sa = { .sa_handler = on_sigint };
    sigemptyset(&sa.sa_mask);
    sigaction(SIGINT, &sa, NULL);

    // listener socket
    int listener = open_listener(LISTEN_PORT);
    if (listener < 0) { perror("listen"); exit(EXIT_FAILURE); }

    // queue & thread-pool 
    pthread_mutex_t queue_mutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_cond_t  queue_cond  = PTHREAD_COND_INITIALIZER;

    queue_t *queue = queue_init(&queue_mutex, &queue_cond);
    if (!queue) { perror("queue_init"); exit(EXIT_FAILURE); }

    struct thread_pool *pool = thread_pool_init(queue, num_threads);
    if (!pool) { perror("thread_pool_init"); exit(EXIT_FAILURE); }

    printf("http_proxy listening on port %d (Ctrl-C to stop)\n", LISTEN_PORT);

    // accept loop 
    while (!shutting_down) {
        int *conn = malloc(sizeof(int));
        if (!conn) continue; // low-mem, try again 

        *conn = accept(listener, NULL, NULL);
        if (*conn < 0) {
            free(conn);
            if (errno == EINTR) continue; // interrupted by SIGINT 
            perror("accept");
            break;
        }

        // wrap the socket in a job_t for the worker threads 
        job_t *job = malloc(sizeof(job_t));
        if (!job) { close(*conn); free(conn); continue; }

        job->func = handle_client_request_thread;
        job->arg  = conn;

        queue_enqueue(queue, job);
    }

    // shutdown sequence 
    close(listener);
    queue_close(queue);        // wake workers so they exit      
    thread_pool_destroy(pool); // joins threads, frees pool     
    queue_destroy(queue);      // free queue storage              
    puts("proxy shut down cleanly");
    return 0;
}
