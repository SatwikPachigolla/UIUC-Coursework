//partner: jeb5
/**
 * teaching_threads
 * CS 241 - Fall 2020
 */
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
 
#include "reduce.h"
#include "reducers.h"
 
static reducer function;
/* You might need a struct for each task ... */
typedef struct _task_container {
    int * start_of_task;
    int * end_of_task;
    int computation_result;
} task_container;
 
/* You should create a start routine for your threads. */
void * compute_task(void * input_ptr) {
    task_container * the_task = (task_container *) input_ptr;
    int * iter = the_task->start_of_task;
    while (iter != the_task->end_of_task) {
        int current_value = *iter;
        the_task->computation_result = function(the_task->computation_result, current_value);
        iter++;
    }
    pthread_exit(NULL);
}
 
int par_reduce(int *list, size_t list_len, reducer reduce_func, int base_case,
               size_t num_threads) {
    /* Your implementation goes here */
    //Allocating space for task list
    function = reduce_func;
    task_container * task_list = malloc(num_threads * sizeof(task_container));
    //Initializing attributes for each task in task list
    size_t index = 0;
    int length = list_len/num_threads;
    int leftover = list_len - (num_threads * length);
    int offset = 0;
    while (index < num_threads) {
        task_list[index].start_of_task = index*(length) + list + offset;
        if ((int) index < leftover) {
            offset++;
        }
        task_list[index].end_of_task = (index+1)*(length) + list + offset;
        task_list[index].computation_result = base_case;
        index++;
    }
    //Declaring my threads and creating them
    pthread_t threads[num_threads];
    index = 0;
    while (index < num_threads) {
        pthread_t * current_thread = threads + index;
        task_container * current_task = task_list + index;
        pthread_create(current_thread, NULL, compute_task, current_task);
        index++;
    }
    //Join the threads back together by end terminating the final thread
    index = 0;
    while(index<num_threads) {
        pthread_join(threads[index], NULL);
        index++;
    }
 
    //Call the reducer function on each task and return the final computation
   
    int final_result = base_case;
    index = 0;
    while (index<num_threads) {
        int computation_result = task_list[index].computation_result;
        final_result = (int) function(final_result, computation_result);
        index++;
    }
   
    free(task_list);
    return final_result;
}