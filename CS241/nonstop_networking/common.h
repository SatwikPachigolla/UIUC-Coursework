/**
 * nonstop_networking
 * CS 241 - Fall 2020
 */
#pragma once
#include <stddef.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdbool.h>
#include "format.h"

#define LOG(...)                      \
    do {                              \
        fprintf(stderr, __VA_ARGS__); \
        fprintf(stderr, "\n");        \
    } while (0);

typedef enum { GET, PUT, DELETE, LIST, V_UNKNOWN } verb;

typedef enum { HEADER, BODY, BAD_REQUEST, BAD_FILE_SIZE, NO_SUCH_FILE } status;

ssize_t write_all_to_socket(int socket, const char *buffer, size_t count);
ssize_t read_all_from_socket(int socket, char *buffer, size_t count);
bool checkErrors(size_t expectedBytes, size_t actualBytes);