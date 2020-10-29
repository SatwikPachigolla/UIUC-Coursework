#include <stdio.h>
#include <stdlib.h>
#include "maze.h"

/*
INTRO PARAGRAPH: We created a program that creates a maze from a given text file(function createMaze)
and parses it to fill in the structure for a maze. Then we created a DFS algorithm to find a valid
solution through the maze. This will mark the solution path with, '*' and visited cells as '~'.(solveMazeDFS)
Then the function printMaze will print solved maze back and destroyMaze will free all of the memory allocated to
solving the maze.

PARTNERS: abhaysp2, sap3
*/


/*
 * createMaze -- Creates and fills a maze structure from the given file
 * INPUTS:       fileName - character array containing the name of the maze file
 * OUTPUTS:      None
 * RETURN:       A filled maze structure that represents the contents of the input file
 * SIDE EFFECTS: None
 */
maze_t * createMaze(char * fileName)
{
    // Your code here. Make sure to replace following line with your own code.

    int i, j, m, n;
    char k;

    maze_t * maze = malloc(sizeof(maze_t)); //allocate memory for maze  struct

    FILE *infile = fopen(fileName, "r"); //open file to read
    fscanf(infile, "%d %d ", &m, &n); //scan in size of maze
    if(infile == NULL){
        return NULL;
    }

    maze->height = m;
    maze->width = n;

    maze->cells =(char **)malloc(m * sizeof(char*)); //allocate mem for cell rows
    for(i=0; i<m; i++){
        maze->cells[i] = malloc(n * sizeof(char)); //allocate mem for cell cols
    }
//parse file to set proper values for cells
    for(i=0; i<(maze->height); i++){
        for(j=0; j<(maze->width)+1; j++){

          fscanf(infile, "%c", &k);

          if(k!= '\n'){ //does not read in the new line character
            (maze->cells[i][j]) = k;
            if(maze->cells[i][j] == START){ //sets start point
                maze->startRow = i;
                maze->startColumn = j;
            }
            if(maze->cells[i][j] == END){ //sets end point
                maze->endRow = i;
                maze->endColumn = j;
            }
          }
        }
    }
    fclose(infile); //close file
    return maze;  //return pointer
}

/*
 * destroyMaze -- Frees all memory associated with the maze structure, including the structure itself
 * INPUTS:        maze -- pointer to maze structure that contains all necessary information
 * OUTPUTS:       None
 * RETURN:        None
 * SIDE EFFECTS:  All memory that has been allocated for the maze is freed
 */
void destroyMaze(maze_t * maze)
{
    // Your code here.
    int i, row;
    row = (maze->height);
    for(i=0; i<row; i++){
        free(maze->cells[i]); //free columns
    }
    free(maze->cells); //free cell rows
    free(maze); //free struct

}

/*
 * printMaze --  Prints out the maze in a human readable format (should look like examples)
 * INPUTS:       maze -- pointer to maze structure that contains all necessary information
 *               width -- width of the maze
 *               height -- height of the maze
 * OUTPUTS:      None    //printf("%d %d\n", maze->height, maze->width);
 * RETURN:       None
 * SIDE EFFECTS: Prints the maze to the console
 */
void printMaze(maze_t * maze)
{
  //prints the maze using double for loop
    int i, j;
    for(i=0; i<(maze->height); i++){
        for(j=0; j<(maze->width); j++){
            printf("%c", maze->cells[i][j]);
        }
        printf("\n");
    }
    // Your code here.
}

/*
 * solveMazeManhattanDFS -- recursively solves the maze using depth first search,
 * INPUTS:               maze -- pointer to maze structure with all necessary maze information
 *                       col -- the column of the cell currently beinging visited within the maze
 *                       row -- the row of the cell currently being visited within the maze
 * OUTPUTS:              None
 * RETURNS:              0 if the maze is unsolvable, 1 if it is solved
 * SIDE EFFECTS:         Marks maze cells as visited or part of the solution path
 */
int solveMazeDFS(maze_t * maze, int col, int row)
{

    //Your code here. Make sure to replace following line with your own code.


    if(col<0 || row<0 || row >= maze->height || col >=  maze->width){ //checks if within bounds
        return 0;
    }
    if(maze->cells[row][col] == END){ //checks if it is the end of the maze
        maze->cells[maze->startRow][maze->startColumn] = START;
        return 1;
    }

    if(maze->cells[row][col] != EMPTY && maze->cells[row][col] != START){ //checks if cell not empty and not start
        return 0;
    }

    maze->cells[row][col] = PATH; //make cell = '*'

    //left
    if(solveMazeDFS(maze, col-1, row) == 1){
        return 1;
    }
    //right
    if(solveMazeDFS(maze, col+1, row) == 1){
        return 1;
    }
    //up
    if(solveMazeDFS(maze, col, row+1) == 1){
        return 1;
    }
    //down
    if(solveMazeDFS(maze, col, row-1) == 1){
        return 1;
    }

    maze->cells[row][col] = VISITED; //unmarks cell and marks as visited

    return 0;
}
