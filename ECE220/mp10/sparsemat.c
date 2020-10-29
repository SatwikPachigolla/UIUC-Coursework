#include "sparsemat.h"

#include <stdio.h>
#include <stdlib.h>


/*
Intro Paragraph: We implemented functions using lists of tuples to handle sparse
matrices with more efficiency, the functions included those which loaded the tuples
from an input_file, a function which handled the modification, deletion, or addition
of nodes, functions to perform matrix arithmetic, and functions to write the resulting
matrices to output files and free up allocated memory

partners: sap3, abhaysp2
*/


//This functions reads an outputted input file and loads the dats of the sparse matrix in
//as a list of tuples. Once it does that, it returns a pointer to the list.
sp_tuples * load_tuples(char* input_file)
{
    FILE *infile = fopen(input_file, "r");
    if(infile == NULL){
      return NULL;
    }

    sp_tuples *mat_ptr = (sp_tuples*)(malloc(sizeof(sp_tuples)));

    int m,n;

    fscanf(infile, "%d %d", &m, &n);

    mat_ptr->m = m;
    mat_ptr->n = n;
    mat_ptr->tuples_head = NULL;
    //mat_ptr->nz = 0;
    int row,col;
    double val;
    while(fscanf(infile, "%d %d %lf", &row, &col, &val) != EOF){
      set_tuples(mat_ptr, row, col, val);
    }
    fclose(infile);
    return mat_ptr;
}


//This function returns the value at the inputted index of the inputted matrix.
//If a node corresponding to the index doesn't exist, it returns 0.
double gv_tuples(sp_tuples * mat_t,int row,int col)

{
    sp_tuples_node *start = mat_t->tuples_head;
    while(start != NULL){
      if(start->row == row && start->col == col){
        return start->value;
      }
      start = start->next;
    }
    return 0;
}


//This function determines and either modifies, adds, or deletes a node given an input
//matrix, index, and value to be inputted. It also tracks the size of the list.
void set_tuples(sp_tuples * mat_t, int row, int col, double value)
{
  sp_tuples_node *prev, *curr;
  //head = mat_t->tuples_head;
  curr = mat_t->tuples_head;
  prev = curr;
  int t_cols = mat_t->n;
  int input_index = (t_cols*row) + col;

  if(curr == NULL && value != 0){
      sp_tuples_node* start = (sp_tuples_node *) malloc(sizeof(struct sp_tuples_node));
      start->next = NULL;
      start->row = row;
      start->col = col;
      start->value = value;
      mat_t->tuples_head=start;
      mat_t->nz = 1;
      return;
  }

  while(curr != NULL){

    int curr_index = ((curr->row)*t_cols) + curr->col;

    if(input_index == curr_index){
        if (value == 0){
            prev->next = curr->next;
            free(curr);
            mat_t->nz --;
            return;
        }
        else{
            curr->value = value;
            return;
        }
    }

    int prev_index = (t_cols * prev->row) + prev->col; //previous node index

    if(input_index < curr_index && input_index > prev_index && value != 0){
        sp_tuples_node* new = (sp_tuples_node *) malloc(sizeof(struct sp_tuples_node));
        new->value = value;
        new->row = row;
        new->col = col;
        new->next = curr;
        prev->next = new;
        mat_t->nz ++;
        return;
    }
    if(prev_index==curr_index && input_index<prev_index && value!=0){
        sp_tuples_node* new_head = (sp_tuples_node *) malloc(sizeof(struct sp_tuples_node));
        new_head->row = row;
        new_head->col = col;
        new_head->value = value;
        new_head->next = curr;
        mat_t->tuples_head = new_head;
        mat_t->nz ++;
        return;
    }
      prev = curr;
      curr = curr->next;
  }
  if(value != 0){
    sp_tuples_node* new = (sp_tuples_node *) malloc(sizeof(struct sp_tuples_node));
    new->value = value;
    new->row = row;
    new->col = col;
    prev->next = new;
    new->next = NULL;
    mat_t->nz ++;
    return;
  }
  return;
}


//This function writes the inputted matrix to the specified output file
void save_tuples(char * file_name, sp_tuples * mat_t)
{
  FILE *outfile = fopen(file_name, "w");
  if(outfile == NULL){
    return;
  }

  fprintf(outfile, "%d %d\n", mat_t->m, mat_t->n);

  sp_tuples_node *curr = mat_t->tuples_head;
  while(curr != NULL){
    fprintf(outfile, "%d %d %f\n", curr->row, curr->col, curr->value);
    curr = curr->next;
  }
  fclose(outfile);
	return;
}


//This function performs matrix addition given to input matrices
sp_tuples * add_tuples(sp_tuples * matA, sp_tuples * matB){

  if(matA->m == matB->m && matA->n == matB->n && matA->m >0 && matA->n >0){

    sp_tuples *new = (sp_tuples*)(malloc(sizeof(sp_tuples)));
    new->m = matA->m;
    new->n = matA->n;
    new->tuples_head = NULL;
    new->nz = 0;

    sp_tuples_node *curr = matA->tuples_head;

    while(curr != NULL){
      set_tuples(new, curr->row, curr->col, curr->value);
      curr = curr->next;
    }

    curr = matB->tuples_head;
    while(curr != NULL){
      set_tuples(new, curr->row, curr->col, curr->value+gv_tuples(new,curr->row,curr->col));
      curr = curr->next;
    }
    return new;
  }
	return NULL;
}


//This function performs matrix multiplication given two input matrices
sp_tuples * mult_tuples(sp_tuples * matA, sp_tuples * matB){

    if(matA->n == matB->m && matA->m > 0 && matA->n > 0 && matB->n > 0){
      sp_tuples *new = (sp_tuples*)(malloc(sizeof(sp_tuples)));
      new->m = matA->m;
      new->n = matB->n;
      new->tuples_head = NULL;
      new->nz = 0;

      sp_tuples_node *curr = matA->tuples_head;
      sp_tuples_node *currB = matB->tuples_head;
      while(curr != NULL){
        if(curr->value != 0){
          int iA = curr->row;
          int jA = curr->col;
          currB = matB->tuples_head;
          while(currB != NULL){
            int iB = currB->row;
            int jB = currB->col;
            if(currB->value != 0 && currB->row == jA){
              set_tuples(new, iA, jB, gv_tuples(new, iA, jB) + (gv_tuples(matA, iA, jA) * gv_tuples(matB, iB, jB)));
            }
            currB = currB->next;
          }
        }
      curr = curr->next;
    }
    return new;
  }
    return NULL;
}


//This function frees up all allocated memory associated with the list/matrix passed in
void destroy_tuples(sp_tuples * mat_t){


  if(mat_t != NULL) {

      sp_tuples_node *curr = mat_t->tuples_head;

      sp_tuples_node *destroy = NULL;

      while(curr != NULL) {
          destroy = curr;
          curr = curr->next;
          free(destroy);
      }
      free(mat_t->tuples_head);
  }

return;
}
