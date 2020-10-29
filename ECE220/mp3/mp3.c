#include <stdio.h>
#include <stdlib.h>

//Introduction paragraph
/*This program takes an input from the user. The input must be an integer number between 0 and 40
  This integer refers to the row on pascal triangle. The program then prints the integers in the
  row by calling to the function we wrote to evaluate combinations. The combination functions takes
  in the row and column and returns the value for it.
*/
//partners: sap3, abhaysp2

unsigned long combination(int n, int k);//Declaring combination function to be used

int main()//main function to be called in program
{
  int row = -1//Initializing input variable
  while(row<0 || row>40){//Test inputted int to see if it is within desired range
    printf("Enter a row index: ");//Keep prompting until something within range is inputted
    scanf("%d", &row);//Scan for input once user is prompted
  }

  int i;//Declaring variable to traverse through columns of triangle

  for(i = 0; i<= row; i++){//Loop to print all the values for the inputted row
    unsigned long x = combination(row,i);//Calling combination function to compute value for specific row and column
    printf("%lu", x);//Print computed value
    printf(" ");//Space between outputs as asked
  }
  printf("\n");//Move onto new line once done outputting



  return 0;//return to end program
}

unsigned long combination(int n, int k){//
  double result = 1.0;
  int i;
  for(i=1; i <= k; i++){
    result *= (n+1-i)*1.0/i;
  }
  result += 0.001;//Small increment to overcompensate for rounding errors and since round() doesn't work with gcc's c89
  return (unsigned long)result;//return the result as an unsigned long, which also fixes for rounding

int main()
{
  int row;

  printf("Enter a row index: ");
  scanf("%d",&row);

  // Write your code here

  return 0;
}
