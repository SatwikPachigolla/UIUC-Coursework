#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int helper(char* pathname){
    struct stat buf;
    if (stat(pathname, &buf) == -1) { 
        return -1;
    }
    else if (S_ISDIR(buf.st_mode)) {
        return -2;
    }
    else if (S_ISLNK(buf.st_mode)) {
        return -3;
    }
    return buf.st_size;
}

int main(int argc, char** argv) {
    if(argc!=2) {
        printf("Usage: ./executable-name path-name\n");
        exit(1);
    }
    int result = helper(argv[1]);
    printf("Code returned: %d\n", result);
    return 0;
}
