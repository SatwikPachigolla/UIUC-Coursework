#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>

static int webPorts;
static pthread_mutex_t webM = PTHREAD_MUTEX_INITIALIZER;
static int sshPorts;
static pthread_mutex_t sshM = PTHREAD_MUTEX_INITIALIZER;
static int valid;
static pthread_mutex_t validM = PTHREAD_MUTEX_INITIALIZER;
static int invalid;
static pthread_mutex_t invalidM = PTHREAD_MUTEX_INITIALIZER;

void incrementPort(const char* port){
    if(!strcmp(port, "22")){
        pthread_mutex_lock(&sshM);
        sshPorts++;
        pthread_mutex_unlock(&sshM);
    }
    else{
        pthread_mutex_lock(&webM);
        webPorts++;
        pthread_mutex_unlock(&webM);
    }
}

//Some code from referencing: http://cs241.cs.illinois.edu/coursebook/Networking
void processHostname(char* hostname, const char* port) {
    struct addrinfo hints;
    struct addrinfo* infoptr = NULL;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET; // AF_INET means IPv4 only addresses

    // Get the machine addresses
    int result = getaddrinfo(hostname, port, &hints, &infoptr);
    if (result) {
        fprintf(stderr, "%s - invalid host\n", hostname);
        pthread_mutex_lock(&invalidM);
        invalid++;
        pthread_mutex_unlock(&invalidM);
        return;
    }
    pthread_mutex_lock(&validM);
    valid++;
    pthread_mutex_unlock(&validM);
    
    int sfd = socket(infoptr->ai_family, infoptr->ai_socktype, infoptr->ai_protocol);
    if (sfd == -1) {
        fprintf(stderr, "socket()\n");
        exit(1);
    }

    if (connect(sfd, infoptr->ai_addr, infoptr->ai_addrlen) == 0) {
        incrementPort(port);
        printf("%s:%s yes\n", hostname, port);
    }
    else {
        printf("%s:%s no\n", hostname, port);
    }
     
    close(sfd);
    freeaddrinfo(infoptr);
}

void* helper22(void* arg) {
    char* hostname = arg;
    processHostname(hostname, "22");
    return NULL;
}

void* helper80(void* arg) {
    char* hostname = arg;
    processHostname(hostname, "80");
    return NULL;
}

int main(int argc, char** argv) {
    if (argc < 2) {
        printf("Usage: ./executable-name hostname1 [hostname2] [hostname3] ...\n");
        exit(1);
    }

    pthread_t tids22[argc];
    pthread_t tids80[argc];
    size_t index;
    for (index = 1; index<argc; index+=1) {
        pthread_create(&tids22[index], NULL, helper22, argv[index]);
        pthread_create(&tids80[index], NULL, helper80, argv[index]);
    }

    for (index = 1; index<argc; index+=1) {
        pthread_join(tids22[index], NULL);
        pthread_join(tids80[index], NULL);
    }

    printf("Successfully connected to %d web port(s) and %d ssh port(s).\n", webPorts, sshPorts);
    printf("%d valid host name(s), %d valid host name(s).\n", valid/2, invalid/2);
    return 0;
}
