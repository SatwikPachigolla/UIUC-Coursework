/**
 * mad_mad_access_patterns
 * CS 241 - Fall 2020
 */
 //partner: jeb5
#include "tree.h"
#include "utils.h"
#include <string.h>
#include <stdlib.h>
/*
  Look up a few nodes in the tree and print the info they contain.
  This version uses fseek() and fread() to access the data.

  ./lookup1 <data_file> <word> [<word> ...]
*/
int binary_search(long the_offset, char * the_phrase, FILE * the_file);

int main(int argc, char **argv) {
  if (2 >= argc) {
    printArgumentUsage();
    exit(1);
  } else {
    char * my_input = argv[1];
    FILE * my_file = fopen(my_input, "r");
    if (my_file == NULL) {
      openFail(my_input);
      exit(2);
    }
    int the_size = 4;
    int index = 1;
    char str_buffer[the_size];
    fread(str_buffer, index, the_size, my_file);
    if (0 != strncmp("BTRE", str_buffer, 4)) {
      formatFail(my_input);
      exit(2);
    } else {
      for (index = 2; index < argc; index++) {
        if (binary_search((long) the_size, argv[index],  my_file) == 0) {
          printNotFound(argv[index]);
        }
      }
    }
    fclose(my_file);
  }
    return 0;
}

//Helper function for performing the binary search
int binary_search(long the_offset, char * the_phrase, FILE * the_file) {
  if (the_offset >= 1) {
    BinaryTreeNode my_node;
    size_t node_size = sizeof(BinaryTreeNode);
    fseek(the_file, the_offset, SEEK_SET);
    fread(&my_node, node_size, 1, the_file);
    fseek(the_file, node_size + the_offset, SEEK_SET);
    char from_file[100];
    fread(from_file, 100, 1, the_file);
    int comparison_val = strcmp(the_phrase, from_file);
    while(the_offset != 0) {
      if (comparison_val == 0) {
        printFound(from_file, my_node.count, my_node.price);
        return 1;
      } else if (comparison_val < 0) {
        the_offset = my_node.left_child;
      } else {
        the_offset = my_node.right_child;
      }
      fseek(the_file, the_offset, SEEK_SET);
      fread(&my_node, node_size, 1, the_file);
      fseek(the_file, node_size + the_offset, SEEK_SET);
      fread(from_file, 100, 1, the_file);
      comparison_val = strcmp(the_phrase, from_file);
    }
  }
  return 0;
}
