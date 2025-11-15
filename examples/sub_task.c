#include "../src/thread_pool.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

// ? if you need to make a sub tasks from a task, you can use a global queue 
task_queue *global_queue = NULL;

typedef struct {
    int level;
    int task_id;
} recursive_task_args;

void child_task(void *args) {
    recursive_task_args *data = (recursive_task_args *)args;
    printf("  [Level %d] Child task %d executing\n", data->level, data->task_id);
    sleep(1);
    printf("  [Level %d] Child task %d completed\n", data->level, data->task_id);
    free(data);
}

void parent_task(void *args) {
    recursive_task_args *data = (recursive_task_args *)args;
    printf("[Level %d] Parent task %d executing\n", data->level, data->task_id);
    
    if (data->level < 3) {
        printf("[Level %d] Parent task %d spawning 2 child tasks...\n", 
               data->level, data->task_id);
        
        for (int i = 0; i < 2; i++) {
            recursive_task_args *child_args = malloc(sizeof(recursive_task_args));
            child_args->level = data->level + 1;
            child_args->task_id = data->task_id * 10 + i + 1;
            
            add_task(global_queue, child_task, child_args);
        }
    }
    
    sleep(1);
    printf("[Level %d] Parent task %d completed\n", data->level, data->task_id);
    free(data);
}

int main() {

    global_queue = create_task_queue();
    pthread_t *pool = init_thread_pool(4, global_queue, NULL);
    
    printf("Adding 3 initial parent tasks...\n\n");
    for (int i = 0; i < 3; i++) {
        recursive_task_args *args = malloc(sizeof(recursive_task_args));
        args->level = 1;
        args->task_id = i + 1;
        add_task(global_queue, parent_task, args);
    }
    
    printf("processing tasks...\n\n");
    sleep(8); // Wait for tasks to complete
    
    
    return 0;
}
