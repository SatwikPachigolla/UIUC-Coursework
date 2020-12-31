/**
 * deepfried_dd
 * CS 241 - Fall 2020
 */
 //partners: jeb5, joowonk2, eroller2
#include "format.h"
#include "string.h"
#include <signal.h>
#include <unistd.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

static unsigned long total_copied; //total BYTES copied
static int status; //for deciding whether to print_status_report
static FILE * input; //input file
static FILE * output; //output file
static size_t p_count; //blocks to skip on input
static size_t k_count; //blocks to skip on output
static int c_count; //BLOCKS copied
static size_t b_size; //block size
static struct timespec start; //our start time
static int full_in;
static int full_out;
static int partial_in;
static int partial_out;

void printReport() {
    struct timespec curr;
    clock_gettime(CLOCK_REALTIME, &curr);
    double diff = difftime(curr.tv_sec, start.tv_sec);
    print_status_report(full_in, partial_in, full_out, partial_out, total_copied, 
    diff + (((double) (curr.tv_nsec - start.tv_nsec)) / 1000000000) );
    status = 0;
}

//Function for copying necessary bytes over
void copy() {
    int copied_blocks = 0;
    char my_buffer[b_size];
    memset(my_buffer, 0, b_size);
    int signal = 1;
    while (signal) {
        //puts("copy");
        if (status == 1) {
            printReport();
        }
        copied_blocks++;
        int the_bytes = fread(&my_buffer, 1, b_size, input);
        if( the_bytes == 0) {
            break;
        }
        fwrite(my_buffer, 1, the_bytes, output);
        total_copied += the_bytes;
        if (c_count == copied_blocks) {
            full_out++;
            full_in++;
            break;
        } else if ((int) b_size != the_bytes) {
            partial_out++;
            partial_in++;
            break;
        }
        full_out++;
        full_in++;
    }
}

void sig_int_handler(int should_int) {
    if (SIGUSR1 == should_int) {
        status = 1;
    }
}

//Function for initializing all parameters
void initialize(int argc, char **argv) {
    int signal;
    b_size = 512;
    c_count = -1;
    p_count = k_count = 0;
    status = 0;
    input = stdin;
    output = stdout;
    full_in = full_out = partial_in = partial_out = 0;
    total_copied = 0;

    while ((signal = getopt (argc, argv, "i:o:b:c:p:k:")) != -1)
        switch (signal) {
            case 'i':
                input = fopen(optarg, "r");
                if (input == NULL) {
                    print_invalid_input(optarg);
                    exit(1);
                }
                break;
            case 'o':
                output = fopen(optarg, "w+");
                if (output == NULL) {
                    print_invalid_output(optarg);
                    exit(1);
                }
                break;
            case 'b':
                b_size = atoi(optarg);
                break;
            case 'c':
                c_count = atoi(optarg);
                break;
            case 'p':
                p_count = atoi(optarg);
                break;
            case 'k':
                k_count = atoi(optarg);
                break;
            case '?':
                exit(1);
            default:
                abort();
            }
}

int main(int argc, char **argv) {
    signal(SIGUSR1, sig_int_handler);
    clock_gettime(CLOCK_REALTIME, &start);
    initialize(argc, argv);
    if (p_count != 0) {
        if (fseek(input, b_size * p_count, SEEK_SET)) {
            exit(1);
        }
    }
    
    if (k_count != 0) {
        if (fseek(output, b_size * k_count, SEEK_SET)) {
            exit(1);
        }
    }

    copy();
    //sleep(1000);
    printReport();
    //puts("about to close");
    fclose(output);
    fclose(input);
    //puts("closed");
    
    return 0;
}