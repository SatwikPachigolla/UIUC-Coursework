/**
 * password_cracker
 * CS 241 - Fall 2020
 */
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <crypt.h>
#include "cracker2.h"
#include "format.h"
#include "utils.h"
#include "includes/queue.h"

typedef struct task_t {
  char* username;
  char* hashPass;
  char* prefix;
} task_t;

static size_t threadCount;
static task_t* task;
pthread_barrier_t barrier;
static char* password;
pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
static int hashTotal;
static bool found;
static bool done;

void free_task(task_t* task);

void* runner(void *index);

int start(size_t thread_count) {
    threadCount = thread_count;
    pthread_barrier_init(&barrier, NULL, threadCount+1);//


    task = malloc(sizeof(task_t));
    task->username = NULL;
    task->hashPass = NULL;
    task->prefix = NULL;

    pthread_t tid[threadCount];
    for (size_t i = 0; i < threadCount; i++) {
        pthread_create(tid+i, NULL, runner, (void *)(i+1));
    }
    size_t capacity = 0;
    ssize_t numRead = 0;
    char *buffer = NULL;
    numRead = getline(&buffer, &capacity, stdin);
    if (numRead>0 && buffer[numRead-1] == '\n') {
        buffer[numRead-1] = '\0';
    }
    while (numRead != -1) {

        char *username = strtok(buffer, " ");
        task->username = realloc(task->username, strlen(username)+1);
        strcpy(task->username, username);
        char *hashPass = strtok(NULL, " ");
        task->hashPass = realloc(task->hashPass, strlen(hashPass)+1);
        strcpy(task->hashPass, hashPass);
        char *prefix = strtok(NULL, " ");
        task->prefix = realloc(task-> prefix, strlen(prefix)+1);
        strcpy(task->prefix, prefix);

        v2_print_start_user(task->username);
        double startTime = getTime();
        double startCPUTime = getCPUTime();

        pthread_barrier_wait(&barrier); //allow all cracker threads to start

        pthread_barrier_wait(&barrier); //wait for cracker threads to finish

        double elapsedTime = getTime() - startTime;
        double totalCPUTime = getCPUTime() - startCPUTime;
        v2_print_summary(task->username, password, hashTotal, elapsedTime, totalCPUTime, !found);

        pthread_mutex_lock(&m);
        found = false;
        hashTotal = 0;
        pthread_mutex_unlock(&m);

        numRead = getline(&buffer, &capacity, stdin);
        if (numRead>0 && buffer[numRead-1] == '\n') {
            buffer[numRead-1] = '\0';
        }
    }
    done = true;
    pthread_barrier_wait(&barrier);
    free(password);
    free(buffer);
    free_task(task);
    
    for (size_t i = 0; i < threadCount; i++) {
        pthread_join(tid[i], NULL);
    }
    pthread_mutex_destroy(&m);
    pthread_barrier_destroy(&barrier);

    return 0; // DO NOT change the return code since AG uses it to check if your
              // program exited normally
}

void free_task(task_t* task) {
    free(task->username);
    free(task->hashPass);
    free(task->prefix);
    free(task);
}

void *runner(void *index) {
    //Man page stuff
    struct crypt_data cData;
    cData.initialized = 0;
    //End man page stuff
    int threadNumber = (long) index;
    char* prefix = NULL;
    char* hashPass = NULL;
    while(true) {
        pthread_barrier_wait(&barrier);

        if (done) break;
        prefix = realloc(prefix, strlen(task->prefix)+1);
        strcpy(prefix, task->prefix);
        
        hashPass = realloc(hashPass, strlen(task->hashPass)+1);
        strcpy(hashPass, task->hashPass);
        
        long start_index;
        long count;
        
        getSubrange(strlen(prefix) - getPrefixLength(prefix), threadCount, threadNumber, &start_index, &count);
        setStringPosition(prefix+getPrefixLength(prefix), start_index);
        v2_print_thread_start(threadNumber, task->username, start_index, prefix);
        int hashCount = 0;
        char* encryption = NULL;

        for (long i = 0; i < count; i++) {
            encryption = crypt_r(prefix, "xx", &cData);
            hashCount++;
            if (!strcmp(encryption, hashPass)) {
                password = realloc(password, strlen(prefix)+1);
                strcpy(password, prefix);
                pthread_mutex_lock(&m);
                found = true;
                v2_print_thread_result(threadNumber, hashCount, 0);
                hashTotal += hashCount;
                pthread_mutex_unlock(&m);
                break;
            }
            pthread_mutex_lock(&m);
            bool check = found;
            pthread_mutex_unlock(&m);
            if (check) {
                pthread_mutex_lock(&m);
                v2_print_thread_result(threadNumber, hashCount, 1);
                hashTotal += hashCount;
                pthread_mutex_unlock(&m);
                break;
            }
            incrementString(prefix);
        }
        pthread_mutex_lock(&m);
        bool check = found;
        pthread_mutex_unlock(&m);
        if (!check) {
            pthread_mutex_lock(&m);
            v2_print_thread_result(threadNumber, hashCount, 2);
            hashTotal += hashCount;
            pthread_mutex_unlock(&m);
        }
        pthread_barrier_wait(&barrier);
    }
    free(prefix);
    free(hashPass);
    return NULL;
}
