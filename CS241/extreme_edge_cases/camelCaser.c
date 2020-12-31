/**
 * extreme_edge_cases
 * CS 241 - Fall 2020
 */
#include "camelCaser.h"
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
#include <string.h>

char **camel_caser(const char *input_str) {
    if(!input_str) {
        return NULL;
    }
    //count sentences and allocate space for the pointers
    unsigned int sentenceCount = 0;
    unsigned int index = 0;
    char check;

    while((check = input_str[index++])){
        sentenceCount+=ispunct(check);
    }

    char** result = (char**) malloc((1+sentenceCount) * sizeof(char*));
    memset(result, 0, (1+sentenceCount) * sizeof(char*));

    result[sentenceCount] = NULL;

    //allocating space for the sentence each pointer points to
    index = 0;
    sentenceCount = 0;
    int charCount = 0;
    while((check = input_str[index++])){
        if(ispunct(check)){
            result[sentenceCount] = (char*) malloc(1+charCount);
            result[sentenceCount][charCount] = '\0';
            charCount = 0;
            sentenceCount++;
        }
        else if(!isspace(check)){
            charCount++;
        }
    }

    //copy over data after camelCasing chars indivdually
    index = 0;
    sentenceCount = 0;
    charCount = 0;
    bool caps = false;
    bool firstWord = true;

    while((check = input_str[index++])){
        if(!result[sentenceCount]){
            break; //Trailing words with no punctuation to make them sentence
        }
        if(ispunct(check)){
            charCount = 0;
            sentenceCount++;
            firstWord = true;
        }
        else if(isspace(check)){
            caps = true;
        }
        else{
            char newChar = caps && !firstWord ? toupper(check) : tolower(check);
            result[sentenceCount][charCount] = newChar;
            charCount++;
            caps = false;
            firstWord = false;
        }
    }

    return result;
}

void destroy(char **result) {
    if(!result){
        return;
    }
    char** iter = result;
    while(*iter){
        free(*iter);
        *iter = NULL;
        iter++;
    }
    free(result);
    result = NULL;
}
