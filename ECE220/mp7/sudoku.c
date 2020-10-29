#include "sudoku.h"

//-------------------------------------------------------------------------------------------------
// Start here to work on your MP7
//-------------------------------------------------------------------------------------------------

/*
Intro paragraph: Implements a program to solve a standard sudoku instance. TThe approach taken here
utilizes recursive backtracking in order to find a solution. Unfilled locations in the grid are
represented by having a value of 0. Once a spot in the grid is assigned a number it will contain that
number's value. This was all accomplished by completing the given functions and creating one more helper
function.
*/
//Partners: sap3, abhaysp2

// You are free to declare any private functions if needed.

// Function: unfilled
int unfilled(int sudoku[9][9], int* i, int* j);

// Function: is_val_in_row
// Return true if "val" already existed in ith row of array sudoku.
int is_val_in_row(const int val, const int i, const int sudoku[9][9]) {

  assert(i>=0 && i<9);


  // BEG TODO
  int j;


  //Row index is set, so this loop iterates through all column indexes for that row, checking if the passed in value
  //already exists in the row. Function returns a 1 if so. Otherwise, returns 0.
  for(j=0; j<9; j++){

      if(sudoku[i][j] == val){
          return 1;
      }
  }
  return 0;
  // END TODO
}

// Function: is_val_in_col
// Return true if "val" already existed in jth column of array sudoku.
int is_val_in_col(const int val, const int j, const int sudoku[9][9]) {

  assert(j>=0 && j<9);

  // BEG TODO
  int i;

  //Column index is set, so this loop iterates through all row indexes for that column, checking if the passed in value
  //already exists in the column. Function returns a 1 if so. Otherwise, returns 0.
  for(i=0; i<9; i++){
      if(sudoku[i][j] == val){
          return 1;
      }
  }



  return 0;
  // END TODO
}

// Function: is_val_in_3x3_zone
// Return true if val already existed in the 3x3 zone corresponding to (i, j)
int is_val_in_3x3_zone(const int val, const int i, const int j, const int sudoku[9][9]) {

  assert(i>=0 && i<9);

  // BEG TODO
  int w, x;
  //Change index to the top left corner of the 3x3 square the indexes passed in are in
  int zone_row_start = (i/3) * 3;
  int zone_col_start = (j/3) * 3;
  //Starting from top left of the 3x3, traverse through the whole 3x3 to see if the val passed in is present in it.
  for(w=0; w<3; w++){
      for(x=0; x<3; x++){
          if(sudoku[zone_row_start + w][zone_col_start + x] == val){
              return 1;
          }
      }
  }



  return 0;
  // END TODO
}

// Function: is_val_valid
// Return true if the val is can be filled in the given entry.
int is_val_valid(const int val, const int i, const int j, const int sudoku[9][9]) {

  assert(i>=0 && i<9 && j>=0 && j<9);

  // BEG TODO

  //If the passed in value exists in any of the groupings it is a part of in the sudoku,
  //return 0 indicating the value is not valid. If not, indicate that it is valid by returning 1.
  if(is_val_in_row(val, i, sudoku) || is_val_in_col(val, j, sudoku) || is_val_in_3x3_zone(val, i, j, sudoku)){
      return 0;
  }


  return 1;
  // END TODO
}

//Helper function that finds a unfilled cell and sets the indexes to point to the cell and returns 1 if one is found.
//Returns 0 if not.
int unfilled(int sudoku[9][9], int* i, int* j){
    int row, col;
    //loop to traverse through the whole board
    for(row = 0; row<9; row++){
        for(col = 0; col<9; col++){
            //search for unfilled cell and point towards it, if found
            if(sudoku[row][col] == 0){
                *i = row;
                *j = col;
                return 1;
            }
        }
    }
    return 0;
}

// Procedure: solve_sudoku
// Solve the given sudoku instance.
int solve_sudoku(int sudoku[9][9]) {
    // BEG TODO.
    int i, j, num;
    //If no unfilled cell is found, the sudoku is solved
    if(!unfilled(sudoku, &i, &j)){
        return 1;
    }

    //Loop through testing all possible values in unfilled cell
    for(num=1; num<=9; num++){
        //Place value in cell if it is valid
        if(is_val_valid(num, i, j, sudoku)){
            sudoku[i][j] = num;
            //Call function from within itself, implementing the recursive logic to keep testing values
            //while not making all set values permanent
            if(solve_sudoku(sudoku)){
                //return 1s to get a return a 1 to the place where function was called from
                //once 1 is returned from earlier in the function
                return 1;
            }
            sudoku[i][j] = 0;//Clear the cell if it didn't lead to the solution
        }
    }
  //Return 0 if no possible number in the current cell being pointed to leads toa solution indicating that
  //the most recent cell before this one needs to keep going through numbers to test
  return 0;
  // END TODO.
}

// Procedure: print_sudoku
void print_sudoku(int sudoku[9][9])
{
  int i, j;
  for(i=0; i<9; i++) {
    for(j=0; j<9; j++) {
      printf("%2d", sudoku[i][j]);
    }
    printf("\n");
  }
}

// Procedure: parse_sudoku
void parse_sudoku(const char fpath[], int sudoku[9][9]) {
  FILE *reader = fopen(fpath, "r");
  assert(reader != NULL);
  int i, j;
  for(i=0; i<9; i++) {
    for(j=0; j<9; j++) {
      fscanf(reader, "%d", &sudoku[i][j]);
    }
  }
  fclose(reader);
}
