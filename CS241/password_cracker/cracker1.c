/**
 * password_cracker
 * CS 241 - Fall 2020
 */
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <crypt.h>
#include <stdlib.h>
#include "cracker1.h"
#include "format.h"
#include "utils.h"
#include "includes/queue.h"

static int numRecovered;
static int numFailed;
static queue* q; //=NULL
static pthread_mutex_t mQueue = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t mRecovered = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t mFailed = PTHREAD_MUTEX_INITIALIZER;

typedef struct task_t {
  char* username;
  char* hashPass;
  char* prefix;
} task_t;

void free_task(task_t* task);

void* runner(void *index);

void parseInput();

int start(size_t thread_count) {

    q = queue_create(0);
    parseInput();
    //queue should have tasks populated with NULL terminator at this point

    pthread_t tid[thread_count];
    for (size_t i = 0; i < thread_count; i++) {
        pthread_create(tid+i, NULL, runner, (void *)(i+1));
    }
    for (size_t i = 0; i < thread_count; i++) {
        pthread_join(tid[i], NULL);
    }
    queue_destroy(q);
    v1_print_summary(numRecovered, numFailed);
    pthread_mutex_destroy(&mRecovered);
    pthread_mutex_destroy(&mFailed);
    pthread_mutex_destroy(&mQueue);
    return 0; // DO NOT change the return code since AG uses it to check if your
              // program exited normally
}

void parseInput() {
    //Input parsing like done in previous assignments
    size_t capacity = 0;
    ssize_t numRead = 0;
    char *buffer = NULL;
    numRead = getline(&buffer, &capacity, stdin);
    if (numRead>0 && buffer[numRead-1] == '\n') {
        buffer[numRead-1] = '\0';
    }
    while (numRead != -1) {
        task_t* task = malloc(sizeof(task_t));
        char *username = strtok(buffer, " ");
        task->username = malloc(strlen(username)+1);
        strcpy(task->username, username);
        char *hashPass = strtok(NULL, " ");
        task->hashPass = malloc(strlen(hashPass)+1);
        strcpy(task->hashPass, hashPass);
        char *prefix = strtok(NULL, " ");
        task->prefix = malloc(strlen(prefix)+1);
        strcpy(task->prefix, prefix);
        queue_push(q, task);

        numRead = getline(&buffer, &capacity, stdin);
        if (numRead>0 && buffer[numRead-1] == '\n') {
            buffer[numRead-1] = '\0';
        }
    }
    queue_push(q, NULL); //analogous to string terminator
    free(buffer);
}

void* runner(void *index) {
    //Man page stuff
    struct crypt_data cData;
    cData.initialized = 0;
    //End man page stuff
    int threadNumber = (long) index;
    task_t* task;
    pthread_mutex_lock(&mQueue);
    while ((task = queue_pull(q))) {
        pthread_mutex_unlock(&mQueue);
        v1_print_thread_start(threadNumber, task->username);
        double startTime = getThreadCPUTime();
        int hashCount = 0;
        char* prefix = malloc(strlen(task->prefix)+1);
        strcpy(prefix, task->prefix);
        setStringPosition(prefix+getPrefixLength(prefix), 0);
        char* encryption;
        while (1) {
            encryption = crypt_r(prefix, "xx", &cData);
            hashCount++;
            if (!strcmp(encryption, task->hashPass)) {
                double timeElapsed = getThreadCPUTime() - startTime;
                v1_print_thread_result(threadNumber, task->username, prefix, hashCount, timeElapsed, 0);
                pthread_mutex_lock(&mRecovered);
                numRecovered++;
                pthread_mutex_unlock(&mRecovered);
                break;
            }
            incrementString(prefix);
            if (strncmp(prefix, task->prefix, getPrefixLength(task->prefix))) {
                double timeElapsed = getThreadCPUTime() - startTime;
                v1_print_thread_result(threadNumber, task->username, NULL, hashCount, timeElapsed, 1);
                pthread_mutex_lock(&mFailed);
                numFailed++;
                pthread_mutex_unlock(&mFailed);
                break;
            }
        }
        free(prefix);
        free_task(task);
        pthread_mutex_lock(&mQueue);
    }
    queue_push(q, NULL);
    pthread_mutex_unlock(&mQueue);
    return NULL;
}

void free_task(task_t* task) {
  free(task->username);
  free(task->hashPass);
  free(task->prefix);
  free(task);
}