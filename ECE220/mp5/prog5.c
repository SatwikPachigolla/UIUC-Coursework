
/*
 * The functions that you must write are defined in the header file.
 * Blank function prototypes with explanatory headers are provided
 * in this file to help you get started.
 */
/*
PARTNERS: abhaysp2, sap3

INTRO PARAGRAPH: We created a simple code-breaking game. The user first enters a seed to create
a code of 4 numbers from 1-7 to guess. The user then has 5 guesses to input the correct sequence
of the 4 numbers. The seed is set using the set_seed function. The solution code is set with the
start_game function. After each guess, the player sees if they got any perfect matches, misplaced
matches, or they got the code correct. A perfect match means the number was the same value and in
the same position as the solution code. A misplaced match means the number was in the solution code,
but is not in the correct position. This is handled by the make_guess function.
If the player guesses the correct code, they win. Otherwise, they lose.
*/


#include <stdio.h>
#include <stdlib.h>

#include "prog5.h"


/*
 * You will need to keep track of the solution code using file scope
 * variables as well as the guess number.
 */

static int guess_number;
static int solution1;
static int solution2;
static int solution3;
static int solution4;


/*
 * set_seed -- This function sets the seed value for pseudorandom
 * number generation based on the number the user types.
 * The input entered by the user is already stored in the string seed_str by the code in main.c.
 * This function should use the function sscanf to find the integer seed value from the
 * string seed_str, then initialize the random number generator by calling srand with the integer
 * seed value. To be valid, exactly one integer must be entered by the user, anything else is invalid.
 * INPUTS: seed_str -- a string (array of characters) entered by the user, containing the random seed
 * OUTPUTS: none
 * RETURN VALUE: 0 if the given string is invalid (string contains anything
 *               other than a single integer), or 1 if string is valid (contains one integer)
 * SIDE EFFECTS: initializes pseudo-random number generation using the function srand. Prints "set_seed: invalid seed\n"
 *               if string is invalid. Prints nothing if it is valid.
 */
int
set_seed (const char seed_str[])
{
//    Example of how to use sscanf to read a single integer and check for anything other than the integer
//    "int seed" will contain the number typed by the user (if any) and the string "post" will contain anything after the integer
//    The user should enter only an integer, and nothing else, so we will check that only "seed" is read.
//    We declare
    int seed;
    char post[2];
//    Checks to see if the character is valid to return 1
    if(sscanf (seed_str, "%d%1s", &seed, post) == 1){
      srand(seed);
      return 1;
    }

//    If there is no integer, seed will not be set. If something else is read after the integer, it will go into "post".
//    The return value of sscanf indicates the number of items read succesfully.
//    When the user has typed in only an integer, only 1 item should be read sucessfully.
//    Check that the return value is 1 to ensure the user enters only an integer.
//    Feel free to uncomment these statements, modify them, or delete these comments as necessary.
//    You may need to change the return statement below
    printf("set_seed: invalid seed\n"); //prints if the seed is invalid
    return 0;
}


/*
 * start_game -- This function is called by main.c after set_seed but before the user makes guesses.
 *               This function creates the four solution numbers using the approach
 *               described in the wiki specification (using rand())
 *               The four solution numbers should be stored in the static variables defined above.
 *               The values at the pointers should also be set to the solution numbers.
 *               The guess_number should be initialized to 1 (to indicate the first guess)
 * INPUTS: none
 * OUTPUTS: *one -- the first solution value (between 1 and 7)
 *          *two -- the second solution value (between 1 and 7)
 *          *three -- the third solution value (between 1 and 7)
 *          *four -- the fourth solution value (between 1 and 7)
 * RETURN VALUE: none
 * SIDE EFFECTS: records the solution in the static solution variables for use by make_guess, set guess_number
 */
void
start_game (int* one, int* two, int* three, int* four)
{
  //creates the solution numbers to guess using rand() function. Each number is stored in the static variables
    solution1 = rand() % 7 + 1;
    *one = solution1; //deference one to make the value at that address equal to solution1
    solution2 = rand() % 7 + 1;
    *two = solution2;
    solution3 = rand() % 7 + 1;
    *three = solution3;
    solution4 = rand() % 7 + 1;
    *four = solution4;
    guess_number = 1;
}

/*
 * make_guess -- This function is called by main.c after the user types in a guess.
 *               The guess is stored in the string guess_str.
 *               The function must calculate the number of perfect and misplaced matches
 *               for a guess, given the solution recorded earlier by start_game
 *               The guess must be valid (contain only 4 integers, within the range 1-7). If it is valid
 *               the number of correct and incorrect matches should be printed, using the following format
 *               "With guess %d, you got %d perfect matches and %d misplaced matches.\n"
 *               If valid, the guess_number should be incremented.
 *               If invalid, the error message "make_guess: invalid guess\n" should be printed and 0 returned.
 *               For an invalid guess, the guess_number is not incremented.
 * INPUTS: guess_str -- a string consisting of the guess typed by the user
 * OUTPUTS: the following are only valid if the function returns 1 (A valid guess)
 *          *one -- the first guess value (between 1 and 7)
 *          *two -- the second guess value (between 1 and 7)
 *          *three -- the third guess value (between 1 and 7)
 *          *four -- the fourth color value (between 1 and 7)
 * RETURN VALUE: 1 if the guess string is valid (the guess contains exactly four
 *               numbers between 1 and 7), or 0 if it is invalid
 * SIDE EFFECTS: prints (using printf) the number of matches found and increments guess_number(valid guess)
 *               or an error message (invalid guess)
 *               (NOTE: the output format MUST MATCH EXACTLY, check the wiki writeup)
 */
int
make_guess (const char guess_str[], int* one, int* two,
	    int* three, int* four)
{
//  One thing you will need to read four integers from the string guess_str, using a process
//  similar to set_seed
//  The statement, given char post[2]; and four integers w,x,y,z,
    char post[2];
    int w,x,y,z,i,j;
    int perfect, misplaced = 0;



    if(sscanf(guess_str, "%d%d%d%d%1s", &w, &x, &y, &z, post)==4){ //checks to see if input is 4 numbers

      int guess_array[] = {w,x,y,z}; //init guess array
      int sol_array[] = {solution1, solution2, solution3, solution4}; // init the solution array
      int pair[4][2] = {{0,0},{0,0},{0,0},{0,0}}; //checks to see which matches have already been counted for(avoids overcounting)

      for(i = 0; i<4; i++){
        if(guess_array[i]<1 || guess_array[i]>7){ //checks for range between 1-7
          printf("make_guess: invalid guess\n");
          return 0;
        }
        if(guess_array[i]==sol_array[i]){ //checks if guess and solution are in the correct location and same val
          pair[i][0] = 1;
          pair[i][1] = 1;
          perfect++; //increment perfect match counter
        }
      }
      for(i = 0; i<4; i++){

        for(j = 0; j<4; j++){ //loop through solution to array to see if a guess a misplaced match
          if(guess_array[i]==sol_array[j]&&pair[j][1]+pair[i][0]==0){ //checks to see if there is a misplaced match
            pair[j][1] = 1;
            pair[i][0]= 1;
            misplaced++; //increment misplaced match counter
          }
        }
      }
      //deference
      *one = w;
      *two = x;
      *three = y;
      *four = z;
      printf("With guess %d, you got %d perfect matches and %d misplaced matches.\n", guess_number, perfect, misplaced);
      guess_number++;
      return 1;
    }

    printf("make_guess: invalid guess\n");
    return 0;
}
