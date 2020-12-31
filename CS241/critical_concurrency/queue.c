 //partner: jeb5
/**
 * critical_concurrency
 * CS 241 - Fall 2020
 */
#include "queue.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * This queue is implemented with a linked list of queue_nodes.
 */
typedef struct queue_node {
    void *data;
    struct queue_node *next;
} queue_node;

struct queue {
    /* queue_node pointers to the head and tail of the queue */
    queue_node *head, *tail;

    /* The number of elements in the queue */
    ssize_t size;

    /**
     * The maximum number of elements the queue can hold.
     * max_size is non-positive if the queue does not have a max size.
     */
    ssize_t max_size;

    /* Mutex and Condition Variable for thread-safety */
    pthread_cond_t cv;
    pthread_mutex_t m;
};

queue *queue_create(ssize_t max_size) {
    ssize_t queue_size = sizeof(struct queue);
    struct queue * my_queue = malloc(queue_size);
    my_queue->head = NULL;
    my_queue->max_size = max_size;
    my_queue->size = 0;
    my_queue->tail = NULL;
    pthread_mutex_init(&(my_queue->m), NULL);
    pthread_cond_init(&(my_queue->cv), NULL);
    return my_queue;
}

void queue_destroy(queue *this) {
    while (this->head != NULL) {
        queue_node * to_delete = this->head;
        this->head = this->head->next;
        free(to_delete);
    }
    pthread_mutex_destroy(&(this->m));
    pthread_cond_destroy(&(this->cv));
    free(this);    
}

void queue_push(queue *this, void *data) {
    pthread_mutex_lock(&this->m);
    while (0 < this->max_size && this->max_size <= this->size) {
        pthread_cond_wait(&(this->cv), &(this->m));
    }
    this->size++;
    ssize_t node_size = sizeof(queue_node);
    queue_node * node = malloc(node_size);
    node->next = NULL;
    node->data = data;
    if (this->head == NULL) {
        this->head = node;
        this->tail = node;
    } else {
        this->tail->next = node;
        this->tail = node;
    }
    
    pthread_cond_broadcast(&(this->cv));
    pthread_mutex_unlock(&(this->m));
    
}

void *queue_pull(queue *this) {
    pthread_mutex_lock(&this->m);
    while(0 == this->size) {
        pthread_cond_wait(&(this->cv), &(this->m));
    }

    queue_node * node = this->head;
    void * node_data = node->data;
    this->head = this->head->next;
    this->size--;
    
    if (this->head == NULL) {
        this->tail = NULL;
    }
    free(node);
    pthread_cond_broadcast(&(this->cv));
    pthread_mutex_unlock(&(this->m));
    return node_data;
}
