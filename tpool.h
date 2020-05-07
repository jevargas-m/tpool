/* Threadpool implementation
 * Author: Enrique Vargas (jevargas@seas.upenn.edu)
 *
 * Usage:
 *      int create_threadpool(int num_threads)
 *              RETURNS: pthread_create return value, 0 if no errors
 * 
 *      int add_threadpool_work(void* func(), void *arg)
 *              A user defined function with argument will be queued for
 *              an available thread to process.
 *              calls free(arg) as side effect
 *              RETURNS: 0 if success, -1 if error
 * 
 * Creates a queue of work items as user defined  funtion+argument, 
 *  and relays to available threads.
 */
#ifndef __TPOOL_H__
#define __TPOOL_H__

int create_threadpool(int num_threads);
int destroy_thredpool();
int add_threadpool_work(void* func(), void *arg);

#endif