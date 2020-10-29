/*
 * countLiveNeighbor
 * Inputs:
 * board: 1-D array of the current game board. 1 represents a live cell.
 * 0 represents a dead cell
 * boardRowSize: the number of rows on the game board.
 * boardColSize: the number of cols on the game board.
 * row: the row of the cell that needs to count alive neighbors.
 * col: the col of the cell that needs to count alive neighbors.
 * Output:
 * return the number of alive neighbors. There are at most eight neighbors.
 * Pay attention for the edge and corner cells, they have less neighbors.
 */

 /*
Partners: abhaysp2, sap3

 Intro: We implemented three functions to complete the game of life. The game of life is a grid
 of cells which there are two states, alive or dead. Each cell's next state depends on the current
 states of its neighbors. The countLiveNeighbor function counts how many neighbors are alive at the
 row and column specified. In each step of time, there can 4 different transitions. These depend on
 the state of the cell and the state of the neighbor cells. The board is updated each step with the
 updateBoard function.To know if the transitions are over, we compare the next step board with the
 current board. If they are the same, the game stops.
 */
int countLiveNeighbor(int* board, int boardRowSize, int boardColSize, int row, int col){
	int i, j;
	int liveneighbors = 0; //initial liveneighbor count

	for(i = -1; i<=1; i++){ //loop to change row index by i to check around the cell

		if(row+i<0 || row+i>=boardRowSize){ //checks if the cell we are trying to access is out of the row bounds
			continue;
		}

		for(j=-1; j<=1; j++){ //loop to change column index by j to check around the cell
			if((i==0&&j==0) || col+j<0 || col+j>=boardColSize){ //checks if the cell we are trying to access is out of thecolumn bounds
				continue;
			}
			if(board[boardColSize*(row+i)+col+j]==1){ //checks if the neighbor is alive
				liveneighbors++; //increment count if the neighbor is alive
			}
		}
	}
	return liveneighbors;
}


/*
 * Update the game board to the next step.
 * Input:
 * board: 1-D array of the current game board. 1 represents a live cell.
 * 0 represents a dead cell
 * boardRowSize: the number of rows on the game board.
 * boardColSize: the number of cols on the game board.
 * Output: board is updated with new values for next step.
 */
void updateBoard(int* board, int boardRowSize, int boardColSize) {
	int temp[boardRowSize*boardColSize]; //initial temp board to store next states in order to keep information in board
	int i;

	for(i=0; i<boardRowSize*boardColSize; i++){
		//Use bitwise OR of cell's state and number of live neighbors to see if it
		//equals 3(anytime it is equal to 3, it was be alive in the the next step)
		if((board[i]|countLiveNeighbor(board, boardRowSize, boardColSize, i/boardColSize, i%boardColSize)) == 3){
			temp[i] = 1; //make alive
		}
		else{
			temp[i] = 0;//make dead
		}
	}

// set each cell in board from the temporary board(next step) that was populated
	for(i=0; i<boardRowSize*boardColSize; i++){
		board[i] = temp[i];
	}


}

/*
 * aliveStable
 * Checks if the alive cells stay the same for next step
 * board: 1-D array of the current game board. 1 represents a live cell.
 * 0 represents a dead cell
 * boardRowSize: the number of rows on the game board.
 * boardColSize: the number of cols on the game board.
 * Output: return 1 if the alive cells for next step is exactly the same with
 * current step or there is no alive cells at all.
 * return 0 if the alive cells change for the next step.
 */
int aliveStable(int* board, int boardRowSize, int boardColSize){
	int i;
	for(i=0; i<boardRowSize*boardColSize; i++){
		//Use bitwise OR (same logic as updateBoard) to see if it equals 3(anytime it is equal to 3, it was be alive in the the next step)
		if((board[i]|countLiveNeighbor(board, boardRowSize, boardColSize, i/boardColSize, i%boardColSize))==3){
			if(board[i]==0){
				return 0;//Not stable if a cell that will be alive next is dead now
			}
		}
		else if(board[i]==1){
			return 0;//Not stable if cell that will be dead is alive now
		}
	}
	return 1;//If no cells are unstable, board is stable
}
