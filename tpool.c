/* Threadpool implementation
 * Author: Enrique Vargas (jevargas@seas.upenn.edu)
 * implement functions in tpool.h
 * 
 * Follows condition variable design pattern described in:
 *   "Advanced Programming in the UNIX Environment, W.Stevens, S.Rago"  pp 416
 * 
 */
#include "tpool.h"

#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

struct work_node {
        void* (*func)();
        void *arg;
        struct work_node *next;
};

struct tid_node {
        pthread_t tid;
        struct tid_node *next;
};

struct work_node *work_head, *work_tail;
struct tid_node *tid_head;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t work_available = PTHREAD_COND_INITIALIZER;

/* Each thread executes this function.
 * As long as there are work nodes in the queue an available thread 
 * deques a node from the head, runs it and then free it. 
 */
void* worker_thread(void *arg)
{       
        struct work_node *node_to_process;

        while (1) {
                /* pass locked mutex to cond_wait and then use lock 
                 * for linkedlist manipulation 
                 */
                if (pthread_mutex_lock(&mutex) != 0) {
                        perror("worker_thread: pthread_mutex_lock");
                        exit(EXIT_FAILURE);
                }
                while (work_head == NULL) {
                        if (pthread_cond_wait(&work_available, &mutex) != 0)
                                exit(EXIT_FAILURE);
                }
                node_to_process = work_head;
                work_head = node_to_process->next;
                if (pthread_mutex_unlock(&mutex) != 0) {
                        perror("worker_thread: pthread_mutex_unlock");
                        exit(EXIT_FAILURE);
                }

                /* execute work concurrently*/
                node_to_process->func(node_to_process->arg);
                
                /* dispose argument and node */
                free(node_to_process->arg);
                free(node_to_process);
        }

        return NULL;
}

int add_threadpool_work(void* func(), void *arg)
{
        struct work_node *new_node;

        new_node = malloc(sizeof(struct work_node));
        if (new_node == NULL) {
                perror("add_threadpool_work: could not malloc memory");
                exit(EXIT_FAILURE);
        }
        new_node->next = NULL;
        new_node->func = func;
        new_node->arg = arg;
        
        /* lock linkedlist manipulation */
        if (pthread_mutex_lock(&mutex) != 0)
                return -1;
                        
        /* insert new node at the tail of list */
        if (work_head == NULL)
                work_head = new_node;
        else
                work_tail->next = new_node;
        work_tail = new_node;
        
        if (pthread_mutex_unlock(&mutex) != 0)
                return -1;
        
        /* notify work available in the queue */
        if (pthread_cond_signal(&work_available) != 0)
                return -1;
        
        return 0;
}

int create_threadpool(int num_threads)
{
        int i, ret;
        struct tid_node *new_tid;

        for (i = 0; i < num_threads; i++) {
                new_tid = malloc(sizeof(struct tid_node));
                if (new_tid == NULL) {
                       perror("push_work: could not malloc memory");
                       exit(EXIT_FAILURE);
                }
                new_tid->next = tid_head;
                tid_head = new_tid;
                ret = pthread_create(&new_tid->tid, NULL, worker_thread, NULL);
                if (ret != 0)
                        return ret;
        }

        return 0;
}

int destroy_thredpool()
{
        struct tid_node *n, *tmp;
        struct work_node *n_work, *tmp_work;

        if (pthread_mutex_lock(&mutex) != 0)
                return -1;
        
        n_work = work_head;
        while (n_work != NULL) {
                tmp_work = n_work;
                n_work = n_work->next;
                free(tmp_work);
        }
        work_head = NULL;  /* ensure threads don't work while destroying */
        
        n = tid_head;
        while (n != NULL) {
                tmp = n;
                n = n->next;
                free(tmp);
        }

        if (pthread_mutex_unlock(&mutex) != 0)
                return -1;

        return 0;
}