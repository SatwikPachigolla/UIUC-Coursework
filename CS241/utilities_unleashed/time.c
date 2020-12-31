/**
 * utilities_unleashed
 * CS 241 - Fall 2020
 */
//partner: jeb5
#include <stdio.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include "format.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        print_time_usage();
    }
    pid_t pid = fork();
    if (pid<0) {
        print_fork_failed();
    }
    else if (pid>0) { //parent
        struct timespec start, end;
        clock_gettime(CLOCK_MONOTONIC, &start);

        int status;
        pid_t check = waitpid(pid, &status, 0);

        clock_gettime(CLOCK_MONOTONIC, &end);
        if(check != -1 && WIFEXITED(status) && !WEXITSTATUS(status)){
            double result = end.tv_sec - start.tv_sec + (end.tv_nsec - start.tv_nsec) / 1000000000.0;
            display_results(argv, result);
        }
    }
    else { //child
        execvp(argv[1], argv+1);
        print_exec_failed();
    }
    return 0;
}
