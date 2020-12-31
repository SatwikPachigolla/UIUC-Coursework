/**
 * nonstop_networking
 * CS 241 - Fall 2020
 */
#include "format.h"
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <netdb.h>
#include <errno.h>
#include "common.h"

//Provided
char **parse_args(int argc, char **argv);
verb check_args(char **args);
//typedef enum { GET, PUT, DELETE, LIST, V_UNKNOWN } verb;

//Forward Declarations
int connect_to_server(const char *host, const char *port);




void processResponse(int serverSocket, verb request, char **args) {
    char responseString[7];
    memset(responseString, 0, 7);
    size_t offset = read_all_from_socket(serverSocket, responseString, 3);
    if (!strcmp(responseString, "OK\n")) {
        fprintf(stdout, "%s", "OK\n");
        
        if (request == DELETE || request == PUT) {
            print_success();
        }
        
        else if (request == GET) {
            FILE* writeFile;
            if ( !(writeFile = fopen(args[4], "w")) ) {
                perror(NULL);
                exit(1);
            }
            size_t responseSize;
            read_all_from_socket(serverSocket, (char *)&responseSize, sizeof(responseSize));
            
            size_t count = 0;
            while (count < responseSize + 5) {
                size_t partSize = responseSize + 5 - count;
                if ( partSize > 1024) {
                    partSize = 1024;
                }
                char buf[1025];
                size_t bytesRead = read_all_from_socket(serverSocket, buf, partSize);
                fwrite(buf, 1, bytesRead, writeFile);
                count += bytesRead;
                if (bytesRead == 0) {
                    break;
                }
            }
            if (checkErrors(responseSize, count)) {
                exit(1);    
            }
            fclose(writeFile);
        }

        if (request == LIST) {
            size_t responseSize;
            read_all_from_socket(serverSocket, (char *)&responseSize, sizeof(responseSize));
            char buf[responseSize + 6];
            memset(buf, 0, responseSize + 6);

            size_t bytesRead = read_all_from_socket(serverSocket, buf, responseSize + 5);

            if (checkErrors(responseSize, bytesRead)) {
                exit(1);    
            }
            //removing this to match weird extra leading newline in reference implementation
            // if(bytesRead == 0) {
            //     buf[0] = '\n';
            // }
            fprintf(stdout, "%zu\n%s", bytesRead, buf);
        }
        
    } 
    else {//error or invalid response
        read_all_from_socket(serverSocket, responseString + offset, 6 - offset);
        if ( !strcmp(responseString, "ERROR\n") ) {
            fprintf(stdout, "%s", "ERROR\n");
            char error[50];
            memset(error, 0, 50);
            if ( !(read_all_from_socket(serverSocket, error, 50)>0) ) {
                print_connection_closed();
            }
            print_error_message(error);
        } 
        else {
            print_invalid_response();
        }
    }
}

void writeRequestType(int serverSocket, verb request, char** args) {
    char* writeString;
    if(request == LIST){
        writeString = malloc(strlen(args[2]) + 1 + 1);
        sprintf(writeString, "%s\n", args[2]);
    }
    else{
        writeString = malloc(strlen(args[2]) + 1+ strlen(args[3]) + 1 + 1);
        sprintf(writeString, "%s %s\n", args[2], args[3]);
    }
    ssize_t expectedSize = strlen(writeString);
    if(write_all_to_socket(serverSocket, writeString, expectedSize) < expectedSize){
        print_connection_closed();
        exit(1);
    }
    free(writeString);
}

void putHelper(int serverSocket, char** args) {
    //https://linux.die.net/man/2/stat
    struct stat fileStat;
    if( stat(args[4], &fileStat) == -1 ) {
        exit(1);
    }

    size_t writeSize = fileStat.st_size;
    write_all_to_socket(serverSocket, (char*)&writeSize, sizeof(writeSize));

    FILE* readFile;
    if( !(readFile = fopen(args[4], "r")) ) {
        exit(1);
    }
    
    size_t count = 0;
    while (count < writeSize) {
        ssize_t partSize = writeSize - count;
        if( partSize > 1024 ){
            partSize = 1024;
        }
        char buf[1025];
        fread(buf, 1, partSize, readFile);
        if (write_all_to_socket(serverSocket, buf, partSize) < partSize) {
            print_connection_closed();
            exit(1);
        }
        count += partSize;
    }
    fclose(readFile);
}

int main(int argc, char **argv) {
    
    char** args = parse_args(argc, argv);
    verb request = check_args(args);

    int serverSocket = connect_to_server(args[0], args[1]);
    
    writeRequestType(serverSocket, request, args);

    if(request == PUT){
        putHelper(serverSocket, args);
    }

    if(shutdown(serverSocket, SHUT_WR)) {
        perror(NULL);
    }
 
    processResponse(serverSocket, request, args);

    //cleanup
    shutdown(serverSocket, SHUT_RD);
    close(serverSocket);
    free(args);
}

//from charming chatroom
int connect_to_server(const char *host, const char *port) {
    struct addrinfo h;
    struct addrinfo* r;
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    memset(&h, 0, sizeof(h));
    h.ai_socktype = SOCK_STREAM;
    h.ai_family = AF_INET;
    int return_from_address = getaddrinfo(host, port, &h, &r);
    if (return_from_address) {
        fprintf(stderr, "gai_strerror(%s)\n", gai_strerror(return_from_address));
        exit(1);
    }

    if(connect(serverSocket, r->ai_addr, r->ai_addrlen) == -1) {
        perror(NULL);
        exit(1);
    }
    freeaddrinfo(r);
    return serverSocket;
}

/**
 * Given commandline argc and argv, parses argv.
 *
 * argc argc from main()
 * argv argv from main()
 *
 * Returns char* array in form of {host, port, method, remote, local, NULL}
 * where `method` is ALL CAPS
 */
char **parse_args(int argc, char **argv) {
    if (argc < 3) {
        return NULL;
    }

    char *host = strtok(argv[1], ":");
    char *port = strtok(NULL, ":");
    if (port == NULL) {
        return NULL;
    }

    char **args = calloc(1, 6 * sizeof(char *));
    args[0] = host;
    args[1] = port;
    args[2] = argv[2];
    char *temp = args[2];
    while (*temp) {
        *temp = toupper((unsigned char)*temp);
        temp++;
    }
    if (argc > 3) {
        args[3] = argv[3];
    }
    if (argc > 4) {
        args[4] = argv[4];
    }

    return args;
}

/**
 * Validates args to program.  If `args` are not valid, help information for the
 * program is printed.
 *
 * args     arguments to parse
 *
 * Returns a verb which corresponds to the request method
 */
verb check_args(char **args) {
    if (args == NULL) {
        print_client_usage();
        exit(1);
    }

    char *command = args[2];

    if (strcmp(command, "LIST") == 0) {
        return LIST;
    }

    if (strcmp(command, "GET") == 0) {
        if (args[3] != NULL && args[4] != NULL) {
            return GET;
        }
        print_client_help();
        exit(1);
    }

    if (strcmp(command, "DELETE") == 0) {
        if (args[3] != NULL) {
            return DELETE;
        }
        print_client_help();
        exit(1);
    }

    if (strcmp(command, "PUT") == 0) {
        if (args[3] == NULL || args[4] == NULL) {
            print_client_help();
            exit(1);
        }
        return PUT;
    }

    // Not a valid Method
    print_client_help();
    exit(1);
}
