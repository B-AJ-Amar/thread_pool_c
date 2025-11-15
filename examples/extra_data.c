#include "../src/thread_pool.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define NUM_THREADS 4
#define BUFFER_SIZE 1024


typedef struct {
    char *message;
    int id;
} task_args;

void process_with_buffer(void *args) {
    task_args *task_data = (task_args *)args;
    char *buffer = (char *)thread_extra;
    
    if (buffer) {
        snprintf(buffer, 1024, "[Task %d] Processing: %s", 
                 task_data->id, task_data->message);
        printf("%s\n", buffer);
    } else {
        printf("[Task %d] No buffer available: %s\n", 
               task_data->id, task_data->message);
    }
    
    sleep(1); // simulate work
    
    free(task_data->message);
    free(task_data);
}


int main() {
 
    task_queue *queue = create_task_queue();
    
    // Allocate a buffer for each thread (buffer pool)
    void **extra_data = malloc(NUM_THREADS * sizeof(void *));
    for (int i = 0; i < NUM_THREADS; i++) {
        extra_data[i] = malloc(BUFFER_SIZE);
        printf("Allocated %d-byte buffer for thread %d\n", BUFFER_SIZE, i);
    }
    
    printf("\n");
    
    pthread_t *pool = init_thread_pool(NUM_THREADS, queue, extra_data);
    
    for (int i = 0; i < 12; i++) {
        task_args *args = malloc(sizeof(task_args));
        args->message = malloc(100);
        snprintf(args->message, 100, "Data item #%d", i + 1);
        args->id = i + 1;
        add_task(queue, process_with_buffer, args);
    }
    

    sleep(10);
    
    
    
    return 0;
}
