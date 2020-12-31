//partner: jeb5
/**
 * critical_concurrency
 * CS 241 - Fall 2020
 */
#include "barrier.h"

// The returns are just for errors if you want to check for them.
int barrier_destroy(barrier_t *barrier) {
    pthread_mutex_destroy(&(barrier->mtx));
    pthread_cond_destroy(&(barrier->cv));
    return 0;
}

int barrier_init(barrier_t *barrier, unsigned int num_threads) {
    pthread_mutex_init(&(barrier->mtx), NULL);
    pthread_cond_init(&(barrier->cv), NULL);
    barrier->n_threads = num_threads;
    barrier->count = num_threads;
    barrier->times_used = 0;
    return 0;
}

int barrier_wait(barrier_t *barrier) {

    //handles one loop iteration
    pthread_mutex_lock(&barrier->mtx);

    barrier->count-=1;
    if (barrier->count == 0) {
        barrier->count = barrier->n_threads;
        barrier->times_used++;
        pthread_cond_broadcast(&barrier->cv); //releases all the other threads to continue
    }
    else {
        pthread_cond_wait(&barrier->cv, &barrier->mtx); //first threads get stuck here
    }

    pthread_mutex_unlock(&barrier->mtx);

    return 0;
}
