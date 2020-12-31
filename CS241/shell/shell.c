/**
 * shell
 * CS 241 - Fall 2020
 */
#include "format.h"
#include "shell.h"
#include "vector.h"
#include "sstring.h"
#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <ctype.h>
#include <time.h>
#include <fcntl.h>


typedef struct process {
    char *command;
    pid_t pid;
} process;

static vector* processes;

void loadHistory(vector* history, const char* histPath);

process* new_process(const char* buffer, pid_t pid);

void destroy_process(pid_t pid);

void destroy_processes();

void kill_and_destroy_all();

void ctrlC();

int execute(char* command, int execFlag);

void handle_external_and_logical(char* buffer);

void ps();

void kill_and_destroy_process(pid_t pid);

void stop_process(pid_t pid);

void cont_process(pid_t pid);

void checkBackground();

void destroy_info(process_info* pInfo);

process_info* create_info(char* command, pid_t pid);

int shell(int argc, char *argv[]) {
    
    signal(SIGINT, ctrlC);
    int check = 1;
    int opt;
    char* histFileName = NULL;
    char* inFileName = NULL;
    while((opt = getopt(argc, argv, "h:f:")) != -1){
        if(opt == 'h'){
            histFileName = optarg;
            check+=2;
        }
        else if ( opt == 'f'){
            inFileName = optarg;
            check+=2;
        }
    }
    if (argc != check){
        print_usage();
        exit(1);
    }

    FILE* inFile;
    if(inFileName){
        inFile = fopen(inFileName, "r");
        if(!inFile){
            print_script_file_error();
            exit(1);
        }
    }
    else{
        inFile = stdin;
    }

    vector* history = string_vector_create();
    char* histPath;
    if(histFileName){
        histPath = get_full_path(histFileName);
        loadHistory(history, histPath);
    }
    
    char* buffer = NULL;
    size_t size = 0;
    ssize_t result;
    processes = shallow_vector_create();
    while(1){

        char* path = get_full_path(".");
        print_prompt(path, getpid());
        free(path);
        
        result = getline(&buffer, &size, inFile);

        if (result == -1){
            kill_and_destroy_all();
            break;
        }
        if ( result <= 1 && buffer[0] == '\n' ) {
            continue;
        }

        if (result>0 && buffer[result-1] == '\n') {
            buffer[result-1] = '\0';
            if(inFile != stdin) {
                print_command(buffer);
            }
        }
        
        //built-ins
        if(!strcmp(buffer,"!history")){
            // VECTOR_FOR_EACH(history, thing, {
            // print_history_line(index, (char *) thing);
            // });
            //need index within loop so for each won't work
            for (size_t i = 0; i < vector_size(history); i++) {
                print_history_line(i, (char *) vector_get(history, i));
            }
        }
        else if (buffer[0] == '!' ) {
            int index;
            for(index = vector_size(history) - 1; index>=0; index--){
                char* command = (char*) vector_get(history, index);
                if( buffer[1] == '\0' || !strncmp(buffer+1, command, strlen(buffer+1)) ){
                    vector_push_back(history, command);
                    print_command(command);
                    execute(command, 0);
                    break;
                }
            }
            if(index<0){
                print_no_history_match();
            }
        }
        else if (buffer[0] == '#') {
            size_t n, n_read;
            n_read = sscanf(buffer+1, "%zu", &n);
            if(n_read && n < vector_size(history)){
                char* command = (char*) vector_get(history, n);
                vector_push_back(history, command);
                print_command(command);
                execute(command, 0);
            }
            else{
                print_invalid_index();
            }
        }
        else if (!strcmp(buffer,"exit")){
            kill_and_destroy_all();
            break;
        }
        else if (!strcmp(buffer,"ps")){
            ps();
        }
        else if (!strncmp(buffer,"kill",4)){
            pid_t pid;
            size_t numRead;
            numRead = sscanf(buffer+4, "%d", &pid);
            if(numRead != 1){
                print_invalid_command(buffer);
            }
            else{
                kill_and_destroy_process(pid);
            }
        }
        else if (!strncmp(buffer,"stop",4)){
            pid_t pid;
            size_t numRead;
            numRead = sscanf(buffer+4, "%d", &pid);
            if(numRead != 1){
                print_invalid_command(buffer);
            }
            else{
                stop_process(pid);
            }
        }
        else if (!strncmp(buffer,"cont",4)){
            pid_t pid;
            size_t numRead;
            numRead = sscanf(buffer+4, "%d", &pid);
            if(numRead != 1){
                print_invalid_command(buffer);
            }
            else{
                cont_process(pid);
            }
        }
        else {
            //Part 1 logical operators
            vector_push_back(history, buffer);
            handle_external_and_logical(buffer);
        }

        //reap children
        checkBackground();
    }
    //cleanup
    free(buffer);
    buffer = NULL;
    if(inFileName){
        fclose(inFile);
    }
    //write to history file
    if(histFileName){
        FILE* hFile = fopen(histPath, "w");
        if(hFile){
            VECTOR_FOR_EACH(history, aLine, {
                fprintf(hFile, "%s\n", (char*)aLine);
            });
            fclose(hFile);
        }
        else{
            print_history_file_error();
        }
        free(histPath);
    }
    vector_destroy(history);
    return 0;
}

void ps(){
    checkBackground();
    print_process_info_header();
    VECTOR_FOR_EACH(processes, aProcess, {
        process* p = (process*) aProcess;
        process_info *pInfo = create_info(p->command, p->pid);
        print_process_info(pInfo);
        destroy_info(pInfo);
        
    });
    process_info *pInfo = create_info("./shell", getpid());
    print_process_info(pInfo);
    destroy_info(pInfo);
}

void destroy_info(process_info* p){
    free(p->start_str);
    free(p->time_str);
    free(p->command);
    free(p);
}

process_info* create_info(char* command, pid_t pid){
    
    process_info* result = malloc(sizeof(process_info));
    
    //pid
    result->pid = pid;
    //command
    result->command = malloc(strlen(command)+1);
    strcpy(result->command, command);
    //nthreads, vsize, state
    char path[100];
    snprintf(path, 100, "/proc/%d/status", pid);
    FILE* file = fopen(path,"r");
    if (!file) {
        print_script_file_error();
        exit(1);
    }
    char* buffer = NULL;
    size_t cap = 0;
    ssize_t numRead;
    while((numRead = getline(&buffer, &cap, file)) != -1){
        if(!strncmp(buffer, "State:", 6)) {
            char* state = buffer + 7;
            while(isspace(*(state++)));
            state--;
            result->state = *state;
        } 
        else if (!strncmp(buffer, "VmSize:", 7)) {
            char *vsize = buffer + 8;
            while(isspace(*(vsize++)));
            vsize--;
            result->vsize = strtol(vsize, NULL, 10);
        } 
        else if (!strncmp(buffer, "Threads:", 8)) {
            char *threads = buffer + 9;
            while(isspace(*(threads++)));
            threads--;
            result->nthreads = strtol(threads, NULL, 10);
        }
    }
    fclose(file);
    free(buffer);
    buffer = NULL;
    //start_str, time_str THIS IS GONNA SUCK https://stackoverflow.com/questions/16726779/how-do-i-get-the-total-cpu-usage-of-an-application-from-proc-pid-stat
    snprintf(path, 100, "/proc/%d/stat", pid);
    file = fopen(path,"r");
    if (!file) {
        print_script_file_error();
        exit(1);
    }
    cap = 0;
    getline(&buffer, &cap, file); //Should never be an issue once file exists
    fclose(file);
    char* check = strtok(buffer, " ");
    int column = 1;
    unsigned long userTime;
    unsigned long kernelTime;
    unsigned long long startTime;
    while(check != NULL) {
        if (column == 14) {
            userTime = strtol(check, NULL, 10);
        } 
        else if (column == 15) {
            kernelTime = strtol(check, NULL, 10);
        } 
        else if (column == 22) {
            startTime = strtol(check, NULL, 10);
        }
        check = strtok(NULL, " ");
        column++;
	}
    free(buffer);
    buffer = NULL;
    cap = 0;
    //time_str
    unsigned long totalTime = (userTime + kernelTime)/sysconf(_SC_CLK_TCK);
    char timeBuffer[50];
    execution_time_to_string(timeBuffer, 50, totalTime/60, totalTime%60);
    result->time_str = malloc(strlen(timeBuffer)+1);
    strcpy(result->time_str, timeBuffer);
    //end time_str
    //start start_str
    file = fopen("/proc/stat","r");
    if (!file) {
        print_script_file_error();
        exit(1);
    }
    unsigned long btime;
    while((numRead = getline(&buffer, &cap, file)) != -1){
        if(!strncmp(buffer, "btime", 5)) {
            char* ptr = buffer + 6;
            while(isspace(*(ptr++)));
            ptr--;
            btime = strtol(ptr, NULL, 10);
            char startBuffer[50];
            time_t timeInSeconds = startTime/sysconf(_SC_CLK_TCK) + btime;
            struct tm* tmPtr = localtime(&timeInSeconds);
            time_struct_to_string(startBuffer, 50, tmPtr);
            result->start_str = malloc(strlen(startBuffer)+1);
            strcpy(result->start_str, startBuffer);
            break;
        }
    }
    fclose(file);
    free(buffer);
    
    return result;
}

void checkBackground(){
    pid_t pid;
    int status;
    while ( (pid = waitpid(-1, &status, WNOHANG)) > 0) {
        //Does exit status even matter?
        destroy_process(pid);
    }
}

void kill_and_destroy_process(pid_t pid){
    for (size_t index = 0; index < vector_size(processes); index++) {
        process *p = (process *) vector_get(processes, index);
        if ( p->pid == pid ) {
            //kill
            if(!kill(pid, SIGKILL)){ //shouldn't fail, just may be blocked
                print_killed_process(pid, p->command);
            } 
            //destroy
            free(p->command);
            free(p);
            vector_erase(processes, index);
            return;
        }
    }
    print_no_process_found(pid);
}

void stop_process(pid_t pid){
    VECTOR_FOR_EACH(processes, aProcess, {
        process* p = (process*) aProcess;
        if ( p->pid == pid ) {
            //stop
            if(!kill(pid, SIGSTOP)){
                print_stopped_process(pid, p->command);
            } 
            return;
        }
    });
    print_no_process_found(pid);
}

void cont_process(pid_t pid){
    VECTOR_FOR_EACH(processes, aProcess, {
        process* p = (process*) aProcess;
        if ( p->pid == pid ) {
            //stop
            if(!kill(pid, SIGCONT)){
                print_continued_process(pid, p->command);
            } 
            return;
        }
    });
    print_no_process_found(pid);
}

void handle_external_and_logical(char* buffer) { //https://stackoverflow.com/questions/9084099/re-opening-stdout-and-stdin-file-descriptors-after-closing-them
    sstring* ssbuffer = cstr_to_sstring(buffer);
    vector* parts = sstring_split(ssbuffer, ' ');
    int doFullCommand = 1; //flag to see if not a case with a special operator
    VECTOR_FOR_EACH(parts, aPart, {

        char* operator = (char*) aPart;
        if (!strcmp(operator, "&&")) {
            char *first = strtok(buffer, "&");
            char *second = strtok(NULL, "");
            first[strlen(first)-1] = '\0'; //Replace space with string terminator
            second +=2;//don't want & and ' ' after first &
            if(!execute(first, 0)) {
                execute(second, 0);
            }
            doFullCommand = 0;
            break;
        }
        else if (!strcmp(operator, "||")) {
            char *first = strtok(buffer, "|");
            char *second = strtok(NULL, "");
            first[strlen(first)-1] = '\0'; //Replace space with string terminator
            second +=2;//don't want | and ' ' after first |
            if(execute(first, 0)) {
                execute(second, 0);
            }
            doFullCommand = 0;
            break;
        }
        else if (operator[strlen(operator)-1] == ';') {
            char *first = strtok(buffer, ";");
            char *second = strtok(NULL, "");
            second+=1;//don't want space after ;
            if(strlen(first) && first[strlen(first)-1] == ' '){
                first[strlen(first)-1] = '\0';
            }
            execute(first, 0);
            execute(second, 0);
            doFullCommand = 0;
            break;
        }
        //add week 2 redirection
        else if (!strcmp(operator, ">")) {
            char *first = strtok(buffer, ">");
            char *second = strtok(NULL, ">");
            first[strlen(first)-1] = '\0'; //Replace space with string terminator
            second +=1;//don't want ' ' after >
            int stdout_copy = dup(1);
            fflush(stdout);
            close(1);
            pid_t pid = 0;
            int file = open(second, O_CREAT | O_WRONLY | O_TRUNC , S_IWUSR | S_IRUSR);
            if(file != -1){
                pid = execute(first, 1);
                fflush(stdout);
                close(file);
            }
            else {
                print_redirection_file_error();
            }
            dup2(stdout_copy, 1);
            if (pid) {
                print_command_executed(pid);
            }
            doFullCommand = 0;
            break;
        }
        else if (!strcmp(operator, ">>")) {
            char *first = strtok(buffer, ">");
            char *second = strtok(NULL, ">");
            first[strlen(first)-1] = '\0'; //Replace space with string terminator
            second +=1;//don't want > and ' ' after first >
            int stdout_copy = dup(1);
            fflush(stdout);
            close(1);
            pid_t pid = 0;
            int file = open(second, O_CREAT | O_WRONLY | O_APPEND , S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
            if(file != -1){
                pid = execute(first, 1);
                fflush(stdout);
                close(file);
            }
            else {
                print_redirection_file_error();
            }
            dup2(stdout_copy, 1);
            if (pid) {
                print_command_executed(pid);
            }
            doFullCommand = 0;
            break;
        }
        else if (!strcmp(operator, "<")) {
            char *first = strtok(buffer, "<");
            char *second = strtok(NULL, "<");
            first[strlen(first)-1] = '\0'; //Replace space with string terminator
            second +=1;//don't want ' ' after <
            int stdin_copy = dup(0);
            close(0);
            int file = open(second, O_RDONLY , S_IWUSR | S_IRUSR);
            if(file != -1){
                execute(first, 0);
                close(file);
            }
            else {
                print_redirection_file_error();
            }
            dup2(stdin_copy, 0);
            doFullCommand = 0;
            break;
        }

    });

    if (doFullCommand) {
        execute(buffer, 0);
    }
    vector_destroy(parts);
    sstring_destroy(ssbuffer);
}

process* new_process(const char *buffer, pid_t pid) {
    process *result = (process*) malloc(sizeof(process));
    result->command = (char*) malloc(strlen(buffer)+1);
    strcpy(result->command, buffer);
    result->pid = pid;
    return result;
}

void destroy_process(pid_t pid){
    for (size_t index = 0; index < vector_size(processes); index++) {
        process *p = (process *) vector_get(processes, index);
        if ( p->pid == pid ) {
            free(p->command);
            free(p);
            vector_erase(processes, index);
            break;
        }
    }
}

void destroy_processes(){
    VECTOR_FOR_EACH(processes, aProcess, {
        process* p = (process*) aProcess;
        free(p->command);
        free(p);
    });
    vector_destroy(processes);
}

void kill_and_destroy_all(){
    VECTOR_FOR_EACH(processes, aProcess, {
        process* p = (process*) aProcess;
        kill(p->pid, SIGKILL);
    });
    destroy_processes();
}

void ctrlC(){ //terminate processes running in shell but not shell itself
    for( size_t i = 0; i<vector_size(processes); i++){
        process* p = vector_get(processes, i);
        if( p->pid != getpgid(p->pid)){
            kill(p->pid, SIGKILL);
            destroy_process(p->pid);
        }
    }
}

int execute(char* command, int execFlag){
    if ( !strncmp(command, "cd", 2)) {
        int cdCheck = chdir(command+3);
        if (cdCheck < 0) {
            print_no_directory(command+3);
            return 1;
        }
        return 0;
    }
    fflush(stdout);//flush stdout before forking to avoid double printing

    //fork-exec-wait
    pid_t pid = fork();
    if (pid<0){
        print_fork_failed();
        exit(1);
    }
    else if (pid > 0) { //parent
        process* p = new_process(command, pid);
        vector_push_back(processes, p);
        //background
        if ( command[strlen(command)-1] == '&') {
            if (setpgid(pid, pid) == -1) {
                print_setpgid_failed();
                exit(1);
            }
        }
        else{ 
            //foreground
            if (setpgid(pid, getpid()) == -1) {
                print_setpgid_failed();
                exit(1);
            }
            int status;
            pid_t result_pid = waitpid(pid, &status, 0);
            if (result_pid != -1) {
                destroy_process(result_pid);
                if (!WIFEXITED(status) || WEXITSTATUS(status)) { //check on this later
                    return 1;
                }
                //What should be printed here?
            }
            else{ //waitpid failure
                print_wait_failed();
                exit(1);
            }
        }
        if(execFlag) {
            return pid;
        }
    }
    else { //child
        if (command[strlen(command)-1] == '&') {
            command[strlen(command)-1] = '\0';
        }
        //get things into execvp format and call it. Need to use vector operations because size unknown
        vector *cVector = sstring_split(cstr_to_sstring(command), ' ');
        char *argv[vector_size(cVector)+1];//extra space to terminate argv with NULL
        size_t index = 0;
        VECTOR_FOR_EACH(cVector, argument, {
            argv[index] = (char*) argument;
            index++;
        });
        if (!strcmp(argv[index-1], "")) {
            argv[index-1] = NULL;
        }
        else {
            argv[index] = NULL;
        }
        if(!execFlag){
            print_command_executed(getpid());
        }
        execvp(argv[0], argv);
        //exec fails if gets here
        print_exec_failed(argv[0]);
        exit(1);
    }
    
    return 0;
}

void loadHistory(vector* history, const char* histPath){
    FILE* hFile = fopen(histPath, "r");
    
    if(hFile){
        //read in history from file
        char* buffer = NULL;
        size_t cap = 0;
        ssize_t result;
        while((result = getline(&buffer, &cap, hFile)) != -1){
            if (result>0 && buffer[result-1] == '\n') {
                buffer[result-1] = '\0';
                vector_push_back(history, buffer);
            }
        }
        free(buffer);
        fclose(hFile);
    }
}
