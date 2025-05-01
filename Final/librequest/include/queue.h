 #include <pthread.h>
 #include <stdio.h>
 #include <stdbool.h>

 typedef struct queue queue_t;

 /// @brief Create a queue with the given 
 /// mutex and condition variables.
 /// @param mutex used to synchronize access to critcal sections 
 /// @param cond_var 
 /// @return queue
 extern queue_t* queue_init(pthread_mutex_t* mutex, pthread_cond_t* cond_var);

 /// @brief Add the data to the back of the queue. 
 /// @param que the queue 
 /// @param data the data 
 extern void queue_enqueue(queue_t* que, void* data);

 extern void* queue_dequeue(queue_t* que);

 extern void queue_close(queue_t* que);

 extern int queue_size(queue_t* que);

 extern bool queue_isclose(queue_t* que);

 extern void queue_destroy(queue_t* que);
