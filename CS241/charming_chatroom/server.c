/**
 * charming_chatroom
 * CS 241 - Fall 2020
 */
 //partner: jeb5
#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "utils.h"

#define MAX_CLIENTS 8

void *process_client(void *p);

static volatile int serverSocket;
static volatile int endSession;

static volatile int clientsCount;
static volatile int clients[MAX_CLIENTS];

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

/**
 * Signal handler for SIGINT.
 * Used to set flag to end server.
 */
void close_server() {
    endSession = 1;
    // add any additional flags here you want.
    //I think that's it??
}

/**
 * Cleanup function called in main after `run_server` exits.
 * Server ending clean up (such as shutting down clients) should be handled
 * here.
 */
void cleanup() {
    if (shutdown(serverSocket, SHUT_RDWR) != 0) {
        perror("shutdown():");
    }
    close(serverSocket);

    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i] != -1) {
            if (shutdown(clients[i], SHUT_RDWR) != 0) {
                perror("shutdown(): ");
            }
            close(clients[i]);
        }
    }
}

/**
 * Sets up a server connection.
 * Does not accept more than MAX_CLIENTS connections.  If more than MAX_CLIENTS
 * clients attempts to connects, simply shuts down
 * the new client and continues accepting.
 * Per client, a thread should be created and 'process_client' should handle
 * that client.
 * Makes use of 'endSession', 'clientsCount', 'client', and 'mutex'.
 *
 * port - port server will run on.
 *
 * If any networking call fails, the appropriate error is printed and the
 * function calls exit(1):
 *    - fprtinf to stderr for getaddrinfo
 *    - perror() for any other call
 */
void run_server(char *port) {
    /*QUESTION 1*/
    /*QUESTION 2*/
    /*QUESTION 3*/
    //AF_INET is IPv4 man ip(7)
    //see if 0 works for protocol
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    /*QUESTION 8*/
    //https://stackoverflow.com/questions/14388706/how-do-so-reuseaddr-and-so-reuseport-differ
    int optval = 1;
    if(setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1) {
        perror(NULL);
        exit(1);
    }
    /*QUESTION 4*/
    /*QUESTION 5*/
    /*QUESTION 6*/
    //http://www.qnx.com/developers/docs/6.5.0SP1.update/com.qnx.doc.neutrino_lib_ref/a/addrinfo.html
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_flags = AI_PASSIVE;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    //linked-list result
    struct addrinfo* res;
    //this is where perror is not used
    //https://man7.org/linux/man-pages/man3/getaddrinfo.3.html
    int error = getaddrinfo(NULL, port, &hints, &res);
    if (error) {
        fprintf(stderr, "getaddrinfo(%s)\n", gai_strerror(error));
        freeaddrinfo(res);
        exit(1);
    }
    /*QUESTION 9*/
    if (bind(serverSocket, res->ai_addr, res->ai_addrlen)) {
        perror(NULL);
        freeaddrinfo(res);
        exit(1);
    }
    /*QUESTION 10*/
    if (listen(serverSocket, MAX_CLIENTS)) {
        perror(NULL);
        freeaddrinfo(res);
        exit(1);
    }

    for (size_t i = 0; i < MAX_CLIENTS; i++) {
        clients[i] = -1;//has to be -1 for cleanup code to work
    }

    printf("Listening on file descriptor %d, port %s\n", serverSocket, port);
    while (!endSession) {
        if (clientsCount < MAX_CLIENTS) {
            printf("Waiting for connection...\n");

            struct sockaddr addr;
            socklen_t addrlen = sizeof(addr);
            memset(&addr, 0, sizeof(addr));

            //What if two clients get here with 1 spot left and both accept a client, 
            //this will make it so the index passed into thread can be -1
            int clientDescriptor = accept(serverSocket, &addr, &addrlen);
            if(clientDescriptor == -1){
                perror(NULL);
                exit(1);
            }
            intptr_t index = -1;
            pthread_mutex_lock(&mutex);
            for (int i = 0; i < MAX_CLIENTS; i++) {
                if (clients[i] == -1) {
                    clients[i] = clientDescriptor;

                    // Why do we even display this? Is it just for more details to display when testing
                    char dst[INET_ADDRSTRLEN];
                    if (inet_ntop(AF_INET, &addr, dst, addrlen)) {
                        printf("Client %d joined on %s\n", i, dst);
                    }
                    index = i;
                    break;
                }
            }
            clientsCount++;
            printf("Currently serving %d clients\n", clientsCount);
            pthread_mutex_unlock(&mutex);


            pthread_t tid;
            if (pthread_create(&tid, NULL, process_client, (void *)index)) {
                perror(NULL);
                exit(1);
            }
        }
    }

    freeaddrinfo(res);
    res = NULL;

}

/**
 * Broadcasts the message to all connected clients.
 *
 * message  - the message to send to all clients.
 * size     - length in bytes of message to send.
 */
void write_to_clients(const char *message, size_t size) {
    pthread_mutex_lock(&mutex);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i] != -1) {
            ssize_t retval = write_message_size(size, clients[i]);
            if (retval > 0) {
                retval = write_all_to_socket(clients[i], message, size);
            }
            if (retval == -1) {
                perror("write(): ");
            }
        }
    }
    pthread_mutex_unlock(&mutex);
}

/**
 * Handles the reading to and writing from clients.
 *
 * p  - (void*)intptr_t index where clients[(intptr_t)p] is the file descriptor
 * for this client.
 *
 * Return value not used.
 */
void *process_client(void *p) {
    pthread_detach(pthread_self());
    intptr_t clientId = (intptr_t)p;
    ssize_t retval = 1;
    char *buffer = NULL;

    while (retval > 0 && endSession == 0) {
        retval = get_message_size(clients[clientId]);
        if (retval > 0) {
            buffer = calloc(1, retval);
            retval = read_all_from_socket(clients[clientId], buffer, retval);
        }
        if (retval > 0)
            write_to_clients(buffer, retval);

        free(buffer);
        buffer = NULL;
    }

    printf("User %d left\n", (int)clientId);
    close(clients[clientId]);

    pthread_mutex_lock(&mutex);
    clients[clientId] = -1;
    clientsCount--;
    pthread_mutex_unlock(&mutex);

    return NULL;
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "%s <port>\n", argv[0]);
        return -1;
    }

    struct sigaction act;
    memset(&act, '\0', sizeof(act));
    act.sa_handler = close_server;
    if (sigaction(SIGINT, &act, NULL) < 0) {
        perror("sigaction");
        return 1;
    }

    run_server(argv[1]);
    cleanup();
    pthread_exit(NULL);
}
