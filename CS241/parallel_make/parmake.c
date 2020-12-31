/**
 * Parallel Make
 * CS 241 - Fall 2020
 */
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include "format.h"
#include "graph.h"
#include "parmake.h"
#include "parser.h"
#include "set.h"
#include "queue.h"

pthread_cond_t cv = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mS = PTHREAD_MUTEX_INITIALIZER;//stack mutex
pthread_mutex_t mG = PTHREAD_MUTEX_INITIALIZER;//graph mutex

graph* g;//needs mutex after creation since it can be multi-threaded then

//pre-processing
set* cycles;//used for single-threaded cycle checking
set* visited;//used for single-threaded cycle checking
vector* temp;//temp vector to keep track of tasks to add to queue if there is no cycle

//populated in pre-processing, used in processing

set* totalSet;//all rules to be processed
vector* stack;//rules ready to be processed
int waiting;//number of threads waiting for a new rule to be processed
int numThreads;//limit to waiting threads to indicate that all rules are processed


bool dependencies(vector *neighbors);
bool shouldRun(char* target, vector* neighbors);
void *runner(void *ptr);
void checkCycle_createStack();

// void debug() {
//     char* toPrint;
//     printf("Base Rules: ");
//     while( (toPrint = queue_pull(q)) ) {
//         printf("%s ",toPrint);
//         queue_push(q, toPrint);
//     }
//     queue_push(q, NULL);
//     printf("\n");
//     printf("All Rules: ");
//     vector* allElements = set_elements(totalSet);
//     VECTOR_FOR_EACH(allElements, elem, {
//         toPrint = elem;
//         printf("%s ",toPrint);
//     });
//     printf("\n");
//     vector_destroy(allElements);
// }

int parmake(char *makefile, size_t num_threads, char **targets) {
    if (num_threads == 0) {
        exit(1);
    }
    //pre-process
    numThreads = num_threads;
    g = parser_parse_makefile(makefile, targets);
    stack = shallow_vector_create();
    totalSet = shallow_set_create();
    //cycle detection and queue creation
    checkCycle_createStack();

    // debug queue and totalSet created
    // debug();
    // printf("\n -------------Debug Done -------------- \n\n");

    //start processing
    pthread_t tid[num_threads];
    for (size_t i = 0; i < num_threads; i++) {
        pthread_create(tid+i, NULL, runner, NULL);
    }

    //wait for runners to finish
    for (size_t i = 0; i < num_threads; i++) {
      pthread_join(tid[i], NULL);
    }

    //clean-up
    vector_destroy(stack);
    set_destroy(totalSet);
    pthread_mutex_destroy(&mG);
    pthread_mutex_destroy(&mS);
    pthread_cond_destroy(&cv);
    graph_destroy(g);
    return 0;
}
 
bool shouldRun(char* target, vector* neighbors) {
    //filename not on disk
    if (access(target, F_OK)) {//0 if ok (if on disk)
        return true;
    }
    for (size_t index = 0; index<vector_size(neighbors); index++) {
        char* neighbor = vector_get(neighbors, index);
        if ( access(neighbor, F_OK) ) {//neighbor not on disk
            return true;
        }
        //rule and neighbor on disk 
        struct stat targetStat, neighborStat;
        stat(target, &targetStat);
        stat(neighbor, &neighborStat);
        if (difftime(targetStat.st_mtime, neighborStat.st_mtime) < 0) {//is neighbor newer
            return true;
        }
    }
    return false;
}

bool dependencies(vector *neighbors) {
    VECTOR_FOR_EACH(neighbors, neighbor, {
        char* key = (char*)neighbor;
        rule_t* rule = (rule_t*) graph_get_vertex_value(g, key);
        if ( !(rule->state) ) {
            return false;
        }
    });
    return true;
}

void* vector_pop() {//only pop from stack so just access global
    void** result = vector_back(stack);
    vector_pop_back(stack);
    return *result;
}

void *runner(void *unused) {
    while (true) {
        //concurrency control
        pthread_mutex_lock(&mS);
        char *target;
        while( !(target = vector_pop(stack)) ) {
            vector_push_back(stack, NULL);
            waiting++;
            if (waiting == numThreads) {
                pthread_cond_broadcast(&cv);
                pthread_mutex_unlock(&mS);
                return NULL;
            }
            pthread_cond_wait(&cv, &mS);
            waiting--;
        }
        pthread_mutex_unlock(&mS);

        //rest of the logic is close to single-threaded
        rule_t* rule = (rule_t*) graph_get_vertex_value(g, target);
        pthread_mutex_lock(&mG);
        vector* neighbors = graph_neighbors(g, target);
        pthread_mutex_unlock(&mG);
        bool failed = false;
        if ( shouldRun(target, neighbors) ) {
            VECTOR_FOR_EACH(rule->commands, elem, {
                char* command = (char*)elem;
                if (system(command)) {
                    failed = true;;
                    break;
                }
            });
        }
        
        if ( !failed ) {
            pthread_mutex_lock(&mG);
            vector* antineighbors = graph_antineighbors(g, target);
            rule->state = 1;//signifies rule has been processed successfully (in this case commands were run succesfully)
            pthread_mutex_unlock(&mG);
            //determine the other rules to be added
            VECTOR_FOR_EACH(antineighbors, elem, {
                char* antineighbor = (char*)elem;
                if (set_contains(totalSet, antineighbor)) {
                    pthread_mutex_lock(&mG);
                    vector* checkNeighbors = graph_neighbors(g, antineighbor);
                    bool ready = dependencies(checkNeighbors);
                    pthread_mutex_unlock(&mG);
                    if (ready) {
                        pthread_mutex_lock(&mS);
                        vector_push_back(stack, antineighbor);
                        pthread_cond_signal(&cv);
                        pthread_mutex_unlock(&mS);
                    }
                    vector_destroy(checkNeighbors);
                }
            });
            
            //end determine the other rules to be added
            vector_destroy(antineighbors);
        }
        vector_destroy(neighbors);
    }
    return NULL;
}





bool helper(char* target) {
    bool result = false;
    if (set_contains(visited, target) || set_contains(cycles, target) ) {
        return true;
    } 
    set_add(visited, target);
    vector* neighbors = graph_neighbors(g, target);
    if ( vector_size(neighbors) ) {
        VECTOR_FOR_EACH(neighbors, neighbor, {
            if ( helper(neighbor) ) {
                set_add(cycles, target);
                result = true;
                break;
            }
        });
    }
    else {
        if ( !set_contains(totalSet, target) ) {
            vector_push_back(temp, target);
        }
    }
    if (!result) {
        set_add(totalSet, target);
        set_remove(visited, target);
    }
    vector_destroy(neighbors);
    return result;
}

void checkCycle_createStack() {
    visited = shallow_set_create();
    cycles = shallow_set_create();
    vector* goalRules = graph_neighbors(g, "");
    vector_push_back(stack, NULL);
    VECTOR_FOR_EACH(goalRules, goal, {
        temp = shallow_vector_create();
        if (helper(goal)) {
            print_cycle_failure(goal);
        }
        else{
            VECTOR_FOR_EACH(temp, elem, {
                char* target = (char*)elem;
                vector_push_back(stack, target);
            });
        }
        vector_destroy(temp);
        temp = NULL;
    });
    vector_destroy(goalRules);
    set_destroy(cycles);
    cycles = NULL;
    set_destroy(visited);
    visited = NULL;
}