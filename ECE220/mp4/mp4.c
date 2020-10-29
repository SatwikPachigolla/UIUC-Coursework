#include <stdlib.h>
#include <stdio.h>
/*
We implemented a program using the expected function that takes
two integers an an input and prints out all the semiprimes, if any
given the input is valid. If input is invalid, message is shown and
program terminates
*/
//partners: sap3, abhaysp2

//Declare functions being used
int is_prime(int number);
int print_semiprimes(int a, int b);


int main(){
   int a, b;
   printf("Input two numbers: ");
   scanf("%d %d", &a, &b);
   if( a <= 0 || b <= 0 ){
     printf("Inputs should be positive integers\n");
     return 1;
   }

   if( a > b ){
     printf("The first number should be smaller than or equal to the second number\n");
     return 1;
   }

   // TODO: call the print_semiprimes function to print semiprimes in [a,b] (including a and b)
   print_semiprimes(a, b); //call print_semiprimes
   printf("\n");//Add new line at the end to make it cleaner for repeated use
   return 0; //must return value
}


/*
 * TODO: implement this function to check the number is prime or not.
 * Input    : a number
 * Return   : 0 if the number is not prime, else 1
 */
int is_prime(int number)//Return 1 if the number is prime and return 0 if not
{
    int i;
    //check if divisible by any other number other than itself
    for(i=2; i<=number/2; i++){
        if(number%i == 0){
            return 0;
        }
    }
    return 1;
}


/*
 * TODO: implement this function to print all semiprimes in [a,b] (including a, b).
 * Input   : a, b (a should be smaller than or equal to b)
 * Return  : 0 if there is no semiprime in [a,b], else 1
 */
int print_semiprimes(int a, int b)
{
    int n, k, check= 0;
     //loop through numbers within range
     for(n = a; n<=b; n++){
         //loop through divisors for each number
         for(k = 2; k<= (n-1); k++){
             //check is prime divisors yield a prime quotient with no remainder
             //if so, the number is semiprime and is printed
             if(n%k == 0 && is_prime(k)==1 && is_prime(n/k) == 1){
                 printf("%d ", n);
                 check = 1;
                 break; //break so it doesn't repeat semiprimes
             }
         }
     }
    //returns 0 if no semiprimes exist within range, otherwise returns 1
    return check;
}
