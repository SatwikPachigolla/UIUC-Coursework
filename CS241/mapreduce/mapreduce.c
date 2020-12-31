//partner: jeb5
/**
 * mapreduce
 * CS 241 - Fall 2020
 */
#include <math.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include "utils.h"
 
int main(int argc, char **argv) {
    size_t mapper_count;
    if ( (argc != 6) || (sscanf(argv[5], "%zu", &mapper_count)) != 1 || (mapper_count < 1) ) {
        print_usage(); //usage is ./mapreduce <input_file> <output_file> <mapper_executable> <reducer_executable> <mapper_count>
        return 1;
    }
 
    char* input_file = argv[1];
    char* output_file = argv[2];
    char* mapper_executable = argv[3];
    char* reducer_executable = argv[4];
 
    // Create an input pipe for each mapper.
    int mapper_pipes[mapper_count][2];
    for (size_t i = 0; i < mapper_count; i++) {
        pipe(mapper_pipes[i]);
    }
    // Create one input pipe for the reducer.
    int reducer_pipe[2];
    pipe(reducer_pipe);
    // Open the output file.
    int flags = O_TRUNC | O_WRONLY | O_CREAT;
    mode_t modes = S_IRUSR | S_IWUSR;
    int filedes = open(output_file, flags, modes);
    // Start a splitter process for each mapper.
    pid_t splitters[mapper_count];
    size_t length = 3;//keep length 1 more than digit length
    for (size_t i = 0; i < mapper_count; i++) {
        char index[length];
        sprintf(index, "%zu", i);
        if(strlen(index)+1 == length) {
            length++;
        }
        pid_t pid = fork();
        if (pid<0){
            //nothing to print when fork fails?
            exit(1);
        }
        else if (pid > 0) { //parent
            splitters[i] = pid;
        }
        else { //child
            close(mapper_pipes[i][0]);
            dup2(mapper_pipes[i][1], 1);
            execl("./splitter", "./splitter", input_file, argv[5], index, NULL);//execl failed
            exit(1);
        }
    }
    // Start all the mapper processes.
    pid_t mappers[mapper_count];
    for (size_t i = 0; i < mapper_count; i++) {
        close(mapper_pipes[i][1]);
        pid_t pid = fork();
        if (pid<0){
            //nothing to print when fork fails?
            exit(1);
        }
        else if (pid > 0) { //parent
            mappers[i] = pid;
        }
        else {
            close(reducer_pipe[0]);
            dup2(mapper_pipes[i][0], 0);
            dup2(reducer_pipe[1], 1);
            execl(mapper_executable, mapper_executable, NULL);
            exit(1);//execl failed
        }
    }
    // Start the reducer process.
    close(reducer_pipe[1]);
    pid_t pid = fork();
    if (pid<0){
        //nothing to print when fork fails?
        exit(1);
    }
    if (pid == 0) { //child
        dup2(reducer_pipe[0], 0);
        dup2(filedes, 1);
        execl(reducer_executable, reducer_executable, NULL);
        exit(1);//execl failed
    }
    //parent
    close(reducer_pipe[0]);
    close(filedes);
    // Wait for the reducer to finish.
 
    //ARE THESE NEEDED?
    for (size_t i = 0; i < mapper_count; i++) {
        int exit_status;
        waitpid(splitters[i], &exit_status, 0);
    }
    for (size_t i = 0; i < mapper_count; i++) {
        close(mapper_pipes[i][0]);
        int exit_status;
        waitpid(mappers[i], &exit_status, 0);
    }
    //ARE THESE NEEDED? ^
 
    int exit_status;
    waitpid(pid, &exit_status, 0);
    // Print nonzero subprocess exit codes.
    if (exit_status) {
        print_nonzero_exit_status(reducer_executable, exit_status);
    }
    // Count the number of lines in the output file.
    print_num_lines(output_file);
    return 0;
}