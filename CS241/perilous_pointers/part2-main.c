/**
 * perilous_pointers
 * CS 241 - Fall 2020
 */
#include "part2-functions.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
/**
 * (Edit this function to print out the "Illinois" lines in
 * part2-functions.c in order.)
 */
int main() {
    //first_step
    first_step(81);
    //second_step
    int secondStep = 132;
    second_step(& secondStep);
    //double_step
    int dvalue = 8942;
    int* dPointer = & dvalue;
    double_step(& dPointer);
    //strange_step
    char c;
    *(int *)(&c + 5) = 15;
    strange_step(&c);
    //empty_step
    char empty[4];
    empty[3] = 0;
    empty_step(empty);
    //two_step
    char two[4];
    two[3] = 'u';
    two_step(two, two);
    //three_step
    char three[1];
    three_step(three, three+2, three+4);
    //step_step_step
    char step[3];
    step[0] = 0;
    step[1] = 8;
    step[2] = 16;
    step_step_step(step-1, step-1, step-1);
    //it_may_be_odd
    int odd = 1;
    it_may_be_odd((char*)&odd, odd);
    //tok_step
    char tok[] = "a,CS241";
    tok_step(tok);
    //the_end
    int end = 0x501;
    the_end(&end, &end);
    return 0;
}