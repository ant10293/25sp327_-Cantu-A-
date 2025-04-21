#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include "tasks.h"

static pthread_mutex_t log_mutex = PTHREAD_MUTEX_INITIALIZER;

void write_log(void* arg) {
    char* msg = (char*)arg;
    log_message(msg);
    free(msg);
}

void compute_sum(void* arg) {
    int n = *((int*)arg);
    long sum = 0;
    for (int i = 0; i < n; i++) sum += i;
    char buf[100];
    snprintf(buf, sizeof(buf), "Sum of numbers < %d is %ld", n, sum);
    log_message(buf);
    free(arg);
}

void delay_task(void* arg) {
    struct timespec* ts = ((struct timespec*)arg);
    nanosleep(ts, NULL);
    char buf[100];
    snprintf(buf, sizeof(buf), "Simulated delay of %ld seconds and %ld milliseconds.", ts->tv_sec, ts->tv_nsec / 1000000);
    log_message(buf);
    free(arg);
}

void log_message(const char* message) {
    pthread_mutex_lock(&log_mutex);
    FILE* f = fopen("task_log.txt", "a");
    if (f) {
        time_t now = time(NULL);
        char timebuf[64];
        strftime(timebuf, sizeof(timebuf), "%Y-%m-%d %H:%M:%S", localtime(&now));
        fprintf(f, "[%s] [Thread %d] %s\n", timebuf, gettid(), message);
        fclose(f);
    }
    pthread_mutex_unlock(&log_mutex);
}