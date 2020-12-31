/**
 * charming_chatroom
 * CS 241 - Fall 2020
 */
//partner: jeb5
 
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include "utils.h"
static const size_t MESSAGE_SIZE_DIGITS = 4;

char *create_message(char *name, char *message) {
    int name_len = strlen(name);
    int msg_len = strlen(message);
    char *msg = calloc(1, msg_len + name_len + 4);
    sprintf(msg, "%s: %s", name, message);

    return msg;
}

ssize_t get_message_size(int socket) {
    int32_t size;
    ssize_t read_bytes =
        read_all_from_socket(socket, (char *)&size, MESSAGE_SIZE_DIGITS);
    if (read_bytes == 0 || read_bytes == -1)
        return read_bytes;

    return (ssize_t)ntohl(size);
}

// You may assume size won't be larger than a 4 byte integer
ssize_t write_message_size(size_t size, int socket) {
    size = htonl(size);
    char * size_to_char = (char *) &size;
    ssize_t wb = write_all_to_socket(socket, size_to_char, MESSAGE_SIZE_DIGITS);
    if (wb == 0) {
        return 0;
    } else if (wb == -1) {
        return -1;
    } else {
        ssize_t size_as_size = (ssize_t) size;
        return size_as_size;
    }
    //return 9001;
}

ssize_t read_all_from_socket(int socket, char *buffer, size_t count) {
    // Your Code Here
    ssize_t acca = 0;
    ssize_t rb;
    while (count >= 1) {
        rb = read(socket, buffer, count);
        if (rb == -1) {
            if (errno == EINTR) {
                continue;
            } else if (errno != EINTR) {
                return -1;
            }
        } else if (rb == 0) {
            return 0;
        } else if (rb >= 1) {
            acca += rb;
            count -= rb;
            buffer += rb;
        }
    }
    return acca;
}

ssize_t write_all_to_socket(int socket, const char *buffer, size_t count) {
    // Your Code Here
    ssize_t acca = 0;
    ssize_t wb;
    while (count >= 1) {
        wb = write(socket, buffer, count);
        if (wb == -1) {
            if (errno == EINTR) {
                continue;
            } else if (errno != EINTR) {
                return -1;
            }
        } else if (wb == 0) {
            return 0;
        } else if (wb >= 1) {
            acca += wb;
            count -= wb;
            buffer += wb;
        }
    }
    return acca;
}
