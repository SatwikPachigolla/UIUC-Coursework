//partner: jeb5
/**
 * deadlock_demolition
 * CS 241 - Fall 2020
 */
#include "graph.h"
#include "libdrm.h"
#include "set.h"
#include <pthread.h>

struct drm_t {
    pthread_mutex_t m;
};

static pthread_mutex_t mGlobal = PTHREAD_MUTEX_INITIALIZER;
static graph* g; //= NULL
static set *visited; //=NULL

drm_t *drm_init() {
    drm_t* result = malloc(sizeof(drm_t));
    pthread_mutex_init(&result->m, NULL);
    pthread_mutex_lock(&mGlobal);
    if (!g) {
      g = shallow_graph_create();
    }
    graph_add_vertex(g, result);
    pthread_mutex_unlock(&mGlobal);
    return result;
}

/**
*  @return
*  0 if the specified thread is not able to unlock the given drm.
*  1 if the specified thread is able to unlock the given drm.
**/
int drm_post(drm_t *drm, pthread_t *thread_id) {
    pthread_mutex_lock(&mGlobal);
    int ret_val = 1;
    if (!graph_contains_vertex(g, thread_id) || !graph_contains_vertex(g, drm)) {
        ret_val = 0;
    }
    if (graph_adjacent(g, drm, thread_id)) {
        graph_remove_edge(g, drm, thread_id);
        pthread_mutex_unlock(&drm->m);
    }
    pthread_mutex_unlock(&mGlobal);
    return ret_val;
}

//Following similar algorithm to https://www.geeksforgeeks.org/detect-cycle-undirected-graph/
bool checkCycleHelper(void *v) {
    /**
    if(isThread) {
        neighbors = graph_neighbors(g, v);
    }
    else {
        neighbors = graph_antineighbors(g, v);
    }
    **/
    bool result = false;
    vector * neighbors = NULL;
    if (set_contains(visited, v)) {
            result = true;
            return result;
    } else {
        set_add(visited, v);
        neighbors = graph_neighbors(g,v);
        VECTOR_FOR_EACH(neighbors, neighbor, {
            if (checkCycleHelper(neighbor)) {
                result = true;
                break;
            }
        });
    }

    vector_destroy(neighbors);
    return result;
}

bool checkCycle(void* thread_id) {
    visited = shallow_set_create();
    bool result = checkCycleHelper(thread_id);
    set_destroy(visited);
    visited = NULL;
    return result;
}
/**
* @return :
*   0 if attempting to lock this drm with the specified thread would cause
*deadlock.
*   1 if the drm can be locked by the specified thread.
**/
int drm_wait(drm_t *drm, pthread_t *thread_id) {
    pthread_mutex_lock(&mGlobal);
    graph_add_vertex(g, thread_id); //doesn't duplicate if thread already exists

    //don't lock same drm twice from same thread
    if (graph_adjacent(g, drm, thread_id)) {     
        pthread_mutex_unlock(&mGlobal);
        return 0;
    } else {
        graph_add_edge(g, thread_id, drm);
        if (checkCycle(thread_id)) {
            graph_remove_edge(g, thread_id, drm);
            pthread_mutex_unlock(&mGlobal);
            return 0;
        }
        pthread_mutex_unlock(&mGlobal);
        pthread_mutex_lock(&drm->m);
        pthread_mutex_lock(&mGlobal);
        graph_remove_edge(g, thread_id, drm);
        graph_add_edge(g, drm, thread_id);
        pthread_mutex_unlock(&mGlobal);
        
        return 1;
    }
}

void drm_destroy(drm_t *drm) {
    pthread_mutex_lock(&mGlobal);
    graph_remove_vertex(g, drm);
    pthread_mutex_destroy(&drm->m);
    free(drm);
    pthread_mutex_unlock(&mGlobal);
}
