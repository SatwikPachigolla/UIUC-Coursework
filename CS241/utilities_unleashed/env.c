/**
 * utilities_unleashed
 * CS 241 - Fall 2020
 */
//partner: jeb5
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include "format.h"

int main(int argc, char *argv[]) {
    if (argc < 4) {
        print_env_usage();
    }
    pid_t pid = fork();
    if(pid<0){
        print_fork_failed();
    }
    else if (pid>0){ //parent
        int status;
        waitpid(pid, &status, 0);
    }
    else{ //child do env stuff
        int index = 1;
        while(index<argc-1){
            if (strcmp(argv[index],"--")) { //While still going through env variables
                char* key = strtok(argv[index], "=");
                char* val = strtok(NULL, "=");
                //strspn might be really inefficient compared to is... commands
                //check that key is valid
                char* ptr = key;
                while(*ptr){
                    if(!(isalpha(*ptr) || isdigit(*ptr) || *ptr == '_')){
                        print_env_usage();
                    }
                    ptr++;
                }

                //do check and value assignment
                if (val == NULL){
                    val = "";
                }
                else if(val[0] == '%'){
                    val = getenv(val+1);
                    if(val == NULL){
                        val = "";
                    }
                }
                else{
                    char* ptr = val;
                    while(*ptr){
                        if(!(isalpha(*ptr) || isdigit(*ptr) || *ptr == '_')){
                            print_env_usage();
                        }
                        ptr++;
                    }
                }
                if(setenv(key,val,1)){
                    print_environment_change_failed();
                }
            }
            else { //Encountered --
                execvp(argv[index+1], argv+index+1);
                print_exec_failed();
            }
            index++; 
        }
        print_env_usage();
    }
    return 0;
}
