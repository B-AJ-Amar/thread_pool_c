#include "../src/thread_pool.h"
#include <stdio.h>
#include <unistd.h>

void simple_task(void *args) {
    int *task_id = (int *)args;
    printf("Task %d is being processed by a thread\n", *task_id);
    sleep(1); 
    printf("Task %d completed\n", *task_id);
    free(task_id);
}

int main() {
    
    task_queue *queue = create_task_queue();
    pthread_t *pool = init_thread_pool(4, queue, NULL); // 4 threads
    
    printf("adding 10 tasks to the queue...\n\n");
    for (int i = 0; i < 10; i++) {
        int *task_id = malloc(sizeof(int));
        *task_id = i + 1;
        add_task(queue, simple_task, task_id);
    }
    
    sleep(5); // Wait for tasks to complete
    
    
    return 0;
}
