/**
 * vector
 * CS 241 - Fall 2020
 */
#include "sstring.h"
#include "vector.h"

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <assert.h>
#include <string.h>

struct sstring {
    vector* vec;
};

sstring *cstr_to_sstring(const char *input) {
    sstring* result = malloc(sizeof(sstring));
    result->vec = char_vector_create();
    const char* ptr = input;
    while(*ptr){
        vector_push_back(result->vec, (void*)ptr);
        ptr++;
    }
    return result;
}

char *sstring_to_cstr(sstring *input) {
    char* result = malloc(vector_size(input->vec)+1);
    char* cPtr = result;
    void** ptr;
    for(ptr = vector_begin(input->vec); ptr<vector_end(input->vec); ptr++,cPtr++){
        *cPtr = *(char *)*ptr;
    }
    *cPtr = '\0';
    return result;
}

int sstring_append(sstring *this, sstring *addition) {
    void** ptr;
    for(ptr = vector_begin(addition->vec); ptr<vector_end(addition->vec); ptr++){
        vector_push_back(this->vec, *ptr);
    }
    return vector_size(this->vec);
}

vector *sstring_split(sstring *this, char delimiter) {
    vector* result = string_vector_create();
    sstring* token = malloc(sizeof(sstring));
    token->vec = char_vector_create();
    void** ptr;
    for (ptr = vector_begin(this->vec); ptr<=vector_end(this->vec); ptr++) {
        char* c = *ptr;
        if(ptr == vector_end(this->vec) || *c == delimiter){
            char* cString = sstring_to_cstr(token);
            vector_push_back(result, cString);
            vector_clear(token->vec);
            free(cString);
            cString = NULL;
        }
        else{
            vector_push_back(token->vec, c);
        }
    }
    sstring_destroy(token);
    return result;
}

int sstring_substitute(sstring *this, size_t offset, char *target,
                       char *substitution) {
    if(!target || !substitution){
        return -1;
    }
    size_t placeholder;
    char* check = target;
    for (size_t i = offset; i < vector_size(this->vec); i++) {
        char *c = vector_get(this->vec, i);
        if(*check == *c){
            if(check == target){
                placeholder = i;
            }
            check++;
        }
        else{
            check = target;
        }
        if(!*check){
            //replace at placeholder
            size_t minSize = strlen(substitution) + vector_size(this->vec)- strlen(target);
            vector_reserve(this->vec, minSize);
            size_t count;
            for(count = 0; count<strlen(target);count++){
                vector_erase(this->vec, placeholder);
            }
            
            check = substitution;
            while(*check){
                vector_insert(this->vec, placeholder++, check++);
            }
            return 0;
        }
    }
    return -1;
}

char *sstring_slice(sstring *this, int start, int end) {
    char* result = malloc(end-start+1);
    char* cPtr = result;
    void** ptr;
    for(ptr = vector_begin(this->vec)+start; ptr<vector_begin(this->vec)+end; ptr++,cPtr++){
        *cPtr = *(char *)*ptr;
    }
    *cPtr = '\0';
    return result;
}

void sstring_destroy(sstring *this) {
    vector_destroy(this->vec);
    this->vec = NULL;
    free(this);
    //No need to NULL line for now
}
