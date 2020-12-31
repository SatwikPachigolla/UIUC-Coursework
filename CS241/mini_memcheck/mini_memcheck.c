//partner: jeb5
/**
 * mini_memcheck
 * CS 241 - Fall 2020
 */
#include "mini_memcheck.h"
#include <stdio.h>
#include <string.h>

meta_data *head = NULL;
size_t total_memory_requested = 0;
size_t total_memory_freed = 0;
size_t invalid_addresses = 0;

void *mini_malloc(size_t request_size, const char *filename,
                  void *instruction) {
    if (!request_size){
        return NULL;
    }
    void* result = malloc(sizeof(meta_data) + request_size);
    if( !(result) ){
        return NULL;
    }
    meta_data* newNode = (meta_data*) result;
    total_memory_requested += request_size;
    newNode->filename = filename;
    newNode->instruction = instruction;
    newNode->request_size = request_size;
    newNode->next = head;
    head = newNode;

    return result + sizeof(meta_data);
}

void *mini_calloc(size_t num_elements, size_t element_size,
                  const char *filename, void *instruction) {
    
    size_t request_size = num_elements*element_size;
    void* result = mini_malloc(request_size, filename, instruction);
    if(result){
        memset(result, 0, request_size);
    }
    return result;
}

void *mini_realloc(void *payload, size_t request_size, const char *filename,
                   void *instruction) {
    if(!payload){
        return mini_malloc(request_size, filename, instruction);
    }
    if(!request_size){
        mini_free(payload);
        return NULL;
    }
    //complex case
    meta_data* prev = NULL;
    meta_data* ptr = head;
    while(ptr){
        meta_data* next = ptr->next;
        if( ((void*)ptr)+sizeof(meta_data) == payload ){
            size_t oldSize = ptr->request_size;
            meta_data* result = realloc(ptr, sizeof(meta_data) + request_size);
            if(!result) {
                return NULL;
            }
            if(oldSize>request_size){
                total_memory_freed += oldSize - request_size;
            }
            else if (request_size > oldSize) {
                total_memory_requested += request_size - oldSize;
            }
            result->request_size = request_size;
            result->filename = filename;
            result->instruction = instruction;
            result->next = next;
            if(prev){
                prev->next = result;
            }
            else {
                head = result;
            }
            return ((void*)result)+sizeof(meta_data);
        }
        prev = ptr;
        ptr = next;
    }
    invalid_addresses++;
    return NULL;
}

void mini_free(void *payload) {
    if(!payload){
        return;
    }
    meta_data* prev = NULL;
    meta_data* ptr = head;
    while(ptr){
        meta_data* next = ptr->next;
        if( ((void*)ptr)+sizeof(meta_data) == payload){
            if(prev){
                prev->next = next;
            }
            else {
                head = next;
            }
            total_memory_freed += ptr->request_size;
            free(ptr);
            return;
        }
        prev = ptr;
        ptr = next;
    }
    invalid_addresses++;
}
