/**
 * mad_mad_access_patterns
 * CS 241 - Fall 2020
 */
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include "tree.h"
#include "utils.h"
//partner: jeb5
/*
* Look up a few nodes in the tree and print the info they contain.
* This version uses mmap to access the data.
* 
* ./lookup2 <data_file> <word> [<word> ...]
*/

BinaryTreeNode *binarySearch(char* start, char* word) {
    BinaryTreeNode* curr = (BinaryTreeNode*) (start + BINTREE_ROOT_NODE_OFFSET);
    while(curr != (BinaryTreeNode*) start) {
        if ( strcmp(word, curr->word) == 0 ) {//found
            return curr;
        }
        if ( strcmp(word, curr->word) < 0 ) {//left
            curr = (BinaryTreeNode*) (start + curr->left_child);
        }
        else {//right
            curr = (BinaryTreeNode*) (start + curr->right_child);
        }
    }
    return NULL;
}

int main(int argc, char **argv) {
    if ( argc < 3 ) {
        printArgumentUsage();
        exit(1);
    }
    char* dataFile = argv[1];
    int filedes = open(dataFile, O_RDONLY);
    if ( filedes == -1 ) {
        openFail(dataFile);
        exit(2);
    }
    struct stat buf;
    if ( fstat(filedes, &buf) == -1 ) {
        openFail(dataFile);
        exit(2);
    }
    char* start = mmap(NULL, buf.st_size, PROT_READ, MAP_PRIVATE, filedes, 0);
    if ( start == (void *)-1 ) {
        mmapFail(dataFile);
        exit(2);
    }
    if ( strncmp(start, BINTREE_HEADER_STRING, 4) ) {
        formatFail(dataFile);
        exit(2);
    }
    for (int i = 2; i < argc; i++) {
        BinaryTreeNode* node = binarySearch(start, argv[i]);
        if ( node ) {
            printFound(argv[i], node->count, node->price);
        }
        else {
            printNotFound(argv[i]);
        }
    }
    close(filedes);
    return 0;
}
