CC      := gcc
LINKER  := gcc
CFLAGS  := -Wall -Wextra -O0          
LDFLAGS := -g                         
LIBS    := -lpthread
OBJS := worker_thread.o thread_pool.o main.o queue.o tasks.o

.PHONY: all clean

all: request_server test_queue cv_simple

request_server: $(OBJS)
	$(LINKER) $(LDFLAGS) $^ $(LIBS) -o $@

test_queue: test_queue.o queue.o
	$(LINKER) $(LDFLAGS) $^ -lcmocka -o $@

cv_simple.o: cv_simple.c queue.h queue_internal.h request.h common.h
	$(CC) $(CFLAGS) -c cv_simple.c -o cv_simple.o

cv_simple: cv_simple.o queue.o
	$(LINKER) $(LDFLAGS) $^ $(LIBS) -o cv_simple

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o request_server test_queue cv_simple
