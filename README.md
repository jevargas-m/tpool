# tpool

## Generic Threadpool 
Simple threadpool implementation in C using POSIX pthread library.

## Usage

### First create the threadpool with n threads using:

create_threadpool(int num_threads)

### Add a function to be executed using:

add_threadpool_work(void* func(), void *arg)

This will enqueue the function for execution by the first available thread in the pool.  By definition all the pushed functions are assumed to be independent and threadsafe among each other (i.e. no global variables or common pointers).

As a side effect the threadpool will call free(arg) after executing the function.

### Destroy the threadpool
destroy_thredpool()

Will wait for threads to finish current function execution and free queue memory afterwards.




