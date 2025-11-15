#include "thread_pool.h"
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

pthread_mutex_t mutex_queue;
sem_t           sem_queue;

__thread void *thread_extra = NULL;

pthread_t *init_thread_pool(int num_threads, task_queue *queue, void **extra_data) {

    pthread_mutex_init(&mutex_queue, NULL);
    sem_init(&sem_queue, 0, 0);

    if (queue == NULL)
        queue = create_task_queue();

    pthread_t *threads = (pthread_t *)malloc(num_threads * sizeof(pthread_t));
    if (!threads) {
        fprintf(stderr, "[init_thread_pool] Failed to allocate memory for threads\n");
        return NULL;
    }

    for (int i = 0; i < num_threads; i++) {
        thread_args *args = malloc(sizeof(thread_args));
        if (!args) {
            fprintf(stderr, "[init_thread_pool] Failed to allocate memory for thread args\n");
            free(threads);
            return NULL;
        }

        args->id    = i;
        args->queue = queue;
        args->extra = (extra_data != NULL) ? extra_data[i] : NULL;

        if (pthread_create(&threads[i], NULL, start_thread, args) != 0) {
            fprintf(stderr, "[init_thread_pool] Failed to create thread\n");
            free(args);
            free(threads);
            return NULL;
        }
    }
    return threads;
}

int destroy_thread_pool(int num_threads, pthread_t *threads) {
    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }
    free(threads);
    return 0;
}

void *start_thread(void *args) {
    thread_args *thread_data = (thread_args *)args;
    task_queue  *queue       = thread_data->queue;

    // Set thread-local storage - now accessible by all task functions
    thread_extra = thread_data->extra;

    while (1) {
        sem_wait(&sem_queue);
        task *t = (task *)get_task(queue);
        sem_post(&sem_queue);
        if (t) {
            t->function(t->args);
            free(t);
        }
    }

    // Note: We don't free thread_extra here - user is responsible for cleanup
    free(thread_data);
    return NULL;
}

task_queue *create_task_queue() {
    task_queue *queue = malloc(sizeof(task_queue));
    if (!queue) {
        fprintf(stderr, "[create_task_queue] Failed to create task queue\n");
        return NULL;
    }
    queue->queue_size = 0;
    queue->first      = NULL;
    queue->last       = NULL;
    return queue;
}

int destroy_task_queue(task_queue *queue) {
    if (!queue)
        return -1;

    task_node *current = queue->first;
    while (current) {
        task_node *tmp = current;
        current        = current->next;
        free(tmp);
    }

    free(queue);
    return 0;
}

void add_task(task_queue *queue, void (*function)(void *), void *args) {

    pthread_mutex_lock(&mutex_queue);
    if (!queue) {

        pthread_mutex_unlock(&mutex_queue);
        return;
    }

    task_node *new_node = malloc(sizeof(task_node));
    if (!new_node) {
        fprintf(stderr, "[add_task] Failed to allocate task_node\n");
        return;
    }

    new_node->task.function = function;
    new_node->task.args     = args;
    new_node->next          = NULL;

    if (queue->last) {
        queue->last->next = new_node;
    } else {
        queue->first = new_node;
    }

    queue->last = new_node;
    queue->queue_size++;
    pthread_mutex_unlock(&mutex_queue);

    sem_post(&sem_queue);
}

void *get_task(task_queue *queue) {
    pthread_mutex_lock(&mutex_queue);
    if (!queue || !queue->first) {
        pthread_mutex_unlock(&mutex_queue);
        return NULL;
    }

    task_node *node = queue->first;
    queue->first    = node->next;
    if (!queue->first) {
        queue->last = NULL;
    }

    void *args               = node->task.args;
    void (*function)(void *) = node->task.function;

    task *ret_task = malloc(sizeof(task));
    if (ret_task) {
        ret_task->function = function;
        ret_task->args     = args;
    }

    free(node);
    queue->queue_size--;
    pthread_mutex_unlock(&mutex_queue);

    return ret_task;
}
