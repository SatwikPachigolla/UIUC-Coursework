#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char** argv) {
    if(argc!=2) {
        printf("Usage: ./executable-name path-name\n");
        exit(1);
    }
    char* pathname = argv[1];
    int pipefd[2];
    pipe(pipefd);
    int file;
    if ( !(file = open(pathname, S_IRUSR)) ) {
        return 1;
    }

    char buf[1];
    while ( read(file, buf, 1) == 1 ) {
        if (write(pipefd[1], buf, 1) < 1) {
            return 1;
        }
    }

    return 0;
}

