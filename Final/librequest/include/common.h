#ifndef COMMON_H
#define COMMON_H

#define _GNU_SOURCE
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

extern const char *strerrorname_np(int) __attribute__((__const__));


#if !defined(__GLIBC__) || (__GLIBC__ < 2 || (__GLIBC__ == 2 && __GLIBC_MINOR__ < 36))
static inline const char *strerrorname_np(int errnum)
{
    (void)errnum;
    return "";
}
#endif


#define handle_error_en(en, msg)                                   \
    do { fprintf(stderr, "%s: (%s) %s\n", (msg),                   \
                 strerrorname_np(en), strerror(en));               \
         exit(EXIT_FAILURE); } while (0)

#define handle_error(msg)                                          \
    do { int _e = errno;                                           \
         fprintf(stderr, "%s: (%s) %s\n", (msg),                   \
                 strerrorname_np(_e), strerror(_e));               \
         exit(EXIT_FAILURE); } while (0)


#define LOCK_MTX(mtx)                                              \
    do { int _r = pthread_mutex_lock(mtx);                         \
         if (_r) handle_error_en(_r, "pthread_mutex_lock"); }      \
    while (0)

#define UNLOCK_MTX(mtx)                                            \
    do { int _r = pthread_mutex_unlock(mtx);                       \
         if (_r) handle_error_en(_r, "pthread_mutex_unlock"); }    \
    while (0)

#endif 
