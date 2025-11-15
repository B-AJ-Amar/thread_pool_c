# Thread Pool in C

A simple and efficient thread pool implementation in C using pthreads, mutexes, and semaphores.

## Features

- **Dynamic thread pool**: Create a pool with any number of threads
- **Task queue**: Thread-safe FIFO queue for task management
- **Thread-local storage**: Optional per-thread data using `__thread` for extra context
- **Mutex-based synchronization**: Safe concurrent access to shared resources
- **Semaphore-based signaling**: Efficient thread wake-up mechanism

## Project Structure

```text
thread_pool_c/
├── src/
│   ├── thread_pool.h    # Header file with API definitions
│   └── thread_pool.c    # Implementation
└── README.md
```

## Quick Start

```c
#include "thread_pool.h"
#include <stdio.h>

// Secondary Thread
void my_async_task(void *args) {
    
    int *value = (request *)args;
    // DO SOMETHING
    free(value);
}

// MAIN THREAD
int main() {
    task_queue *queue = create_task_queue();
    
    pthread_t *pool = init_thread_pool(4, queue, NULL); // thread pool with 4 threads
    
    while (1) {
      request *req = server_listen(5000);  // Wait for incoming request
      add_task(queue, handle_request, req); // Assign to a thread 
    }
    
    return 0;
}

```


you can find more exemples [here](./examples/)
