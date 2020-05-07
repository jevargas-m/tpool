/* Threadpool usage example
 * 
 * Author: jevargas-m
 * Hit CTRL+C to terminate
 * 
 * Must show interleaving work executed concurrently:
 * 
 * Note:
 *      1) Work is started for NUM_THREADS and happens concurrently
 *      2) NUM_WORK_FUNCTIONS are enqueued at the beginning even with
 *         threads working concurrently.
 *      3) The moment work finishes a pending work is started, at any
 *         time NUM_THREADS are working at the same time.
 * 
 * Sample Output:
 *      Work #0 enqueued
 *      Work #0 started
 *      Work #1 enqueued
 *      Work #2 enqueued
 *      Work #3 enqueued
 *      Work #4 enqueued
 *      Work #5 enqueued
 *      Work #6 enqueued
 *      Work #7 enqueued
 *      Work #8 enqueued
 *      Work #1 started
 *      Work #9 enqueued
 *      Work #2 started
 *      Work #0 finished after 3 seconds of work
 *      Work #3 started
 *      Work #1 finished after 6 seconds of work
 *      Work #4 started
 *      Work #2 finished after 7 seconds of work
 *      Work #5 started
 *      Work #3 finished after 5 seconds of work
 *      Work #6 started
 *      Work #4 finished after 3 seconds of work
 *      Work #7 started
 *      Work #7 finished after 2 seconds of work
 *      Work #8 started
 *      Work #5 finished after 5 seconds of work
 *      Work #9 started
 *      Work #9 finished after 1 seconds of work
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <assert.h>
#include "../tpool.h"

#define NUM_THREADS 3
#define NUM_WORK_FUNCTIONS 10

/* This ilustrates how to pass more than one argument */
struct func_arg {
        int id;
        int time_to_sleep;
};

/* function to pass to threadpool */
void* my_important_work(void* arg)
{
        struct func_arg *my_arg = (struct func_arg*) arg;
        
        printf("Work #%d started\n", my_arg->id);
        sleep(my_arg->time_to_sleep); /* just sleep */
        printf("Work #%d finished after %d seconds of work\n", 
               my_arg->id, my_arg->time_to_sleep);

        return NULL; /* must return a void* */
}

void sigint_handler(int sig)
{    
        destroy_thredpool();
        printf("Threadpool destroyed ..\n");
        exit(EXIT_SUCCESS);   
}

/* Program adds work to threadpool and spin locks until CTRL+C is pressed */
int main()
{
        int i;

        /* destroy threadpool on CTRL + C */
        if (signal(SIGINT, sigint_handler) == SIG_ERR) {
                perror("SIGINT signal");
                exit(EXIT_FAILURE);
        }
        if (create_threadpool(NUM_THREADS)) {
                perror("create_threadpool");
                exit(EXIT_FAILURE);
        } 
        for (i = 0; i < NUM_WORK_FUNCTIONS; i++) {
                struct func_arg *a = malloc (sizeof(struct func_arg));

                assert(a != NULL);
                a->id = i;
                a->time_to_sleep = rand() % 10;  /* random time [0..9] secs */
                if (add_threadpool_work(my_important_work, (void*) a)) {
                        perror("add_threadpool_word");
                        exit(EXIT_FAILURE);  
                }
                printf("Work #%d enqueued\n", i);
        }
        while(1);  /* spin lock to keep main alive and show threads working */

        return 0;
}