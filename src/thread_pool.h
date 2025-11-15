#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct task {
    void (*function)(void *);
    void *args;
} task;

typedef struct task_node {
    task              task;
    struct task_node *next;
} task_node;

typedef struct task_queue {
    int        queue_size;
    task_node *first;
    task_node *last;
} task_queue;

typedef struct thread_args {
    int         id;
    task_queue *queue;
    void       *extra; // extra thread specific data
} thread_args;

extern __thread void *thread_extra;

pthread_t  *init_thread_pool(int num_threads, task_queue *queue, void **extra_data);
int         destroy_thread_pool(int num_threads, pthread_t *threads);
void       *start_thread(void *args);
task_queue *create_task_queue();
int         destroy_task_queue(task_queue *queue);
void        add_task(task_queue *queue, void (*function)(void *), void *args);
void       *get_task(task_queue *queue);

#endif // THREAD_POOL_H