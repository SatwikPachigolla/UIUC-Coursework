/**
 * nonstop_networking
 * CS 241 - Fall 2020
 */
#include "common.h"

//Charming chatroom functions written by partner (jeb5) for the lab assignment


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
            break;
        } else if (wb >= 1) {
            acca += wb;
            count -= wb;
            buffer += wb;
        }
    }
    return acca;
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
            break;
        } else if (rb >= 1) {
            acca += rb;
            count -= rb;
            buffer += rb;
        }
    }
    return acca;
}

bool checkErrors(size_t expectedBytes, size_t actualBytes) {
	
	if (actualBytes == 0 && expectedBytes) {
		print_connection_closed();
		return true;
	} 
	if (actualBytes > expectedBytes) {
		print_received_too_much_data();
		return true;
	}
	if (actualBytes < expectedBytes) {
		print_too_little_data();
		return true;
	}
	return false;
}
