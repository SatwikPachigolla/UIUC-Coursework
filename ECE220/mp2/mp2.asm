; mp2.asm
; Implement a stack calculator that can do the following operations:
; ADD, SUBTRACT, MULTIPLY, DIVIDE


;INTRO PARAGRAPH: We have created a basic stack calculator that only takes integers from 0-9 and outputs the value
;in hexadecimal. The calculator has 4 operators, (Addition, Subtraction, Division, Multiplication). The input must be
;written in Postfix notation for the calculator to properly read the characters. If the value that is read is an operand,
;the calculator will push it onto the stack. If the value that is read is an operator, it will pop two values from the stack then check
;for underflow and apply the operator to the two values. This is repeated until a '=' is detected.
;A stack underflow is automatically an invalid expression. Also, if there is more than one value in the stack, it is invalid
;The evaluated value then is printed in hex with the PRINT_HEX subroutine and the value is stored in R5

; Partners: sap3, abhaysp2
; Abhay Patel abhaysp2
; Satwik Pachigolla sap3

; Inputs:
;   Console - postfix expression

; Outputs:
;   Console - [0 - F]{4}, evaluation of postfix expression
;   Register R5 - [0 - F]{4}, evaluation of postfix expression

.ORIG x3000


MAIN

JSR EVALUATE ;Runs evaluate subroutine

JSR PRINT_HEX
;Print the evaluated result to the monitor if it is a valid expression

DONE
		HALT

; PRINT_HEX
;   Description: prints out the value in R3 to the console
;   Inputs: R3 - value in register to be printed out
;   Outputs: Value printed to the console
PRINT_HEX
						ST R7, PRINT_SAVER7 ;save R7
						ST R5, PRINT_SAVER5 ;save R7
						AND R3, R3, #0
						ADD R3, R3, R5; Setting the input for the print to take in the evaluated value

            AND R1, R1, #0 ;initialize digit counter
            ADD R1, R1, #4 ;set digit counter to 4

NEXT_DIGIT  AND R6, R6, #0
            ADD R1, R1, #0
            BRz PRINTED

            AND R2, R2, #0 ;init bit counter
            ADD R2, R2, #4 ;set bit counter to 4

            AND R4, R4, #0 ;init digit register

NEXT_BIT    ADD R2, R2, #0
            BRz ASCII ; checks if got < 4 bits from R3

            ADD R4, R4, R4 ;Shift digit to left

            ADD R3, R3, #0
            BRn NEGATIVE ;checks if it is a 0 or a 1
            ADD R4, R4, #0 ;Add 0 to digit
            BRnzp BREAK

NEGATIVE    ADD R4, R4, #1 ;Add 1 to digit

BREAK       ADD R3, R3, R3 ; Shift R3 to the left
            ADD R2, R2, #-1 ; Decrement bit counter
            BRnzp NEXT_BIT

						;checks if <= 9
ASCII       AND R5, R5, #0
            ADD R5, R4, #-9
            BRp NINE

						;if <= 9 it will do this
            LD R6, ZERO
            ADD R4, R4, R6
            BRnzp JUMP

						;if >9 it will do this
NINE        LD R6, A
            ADD R4, R4, R6
            ADD R4, R4, #-10

						;Stores the ASCII value of digit
JUMP        AND R0, R0, #0
						ADD R0, R4, #0
            OUT

            ADD R1, R1, #-1 ;decrements digit counter
            BRnzp NEXT_DIGIT

PRINTED
						LD R7, PRINT_SAVER7
						LD R5, PRINT_SAVER5
						RET

PRINT_SAVER7 .BLKW #1
PRINT_SAVER5 .BLKW #1
ZERO .FILL x0030
A .FILL x0041




; EVALUATE
;   Description: handles input from console
;   Inputs: Console - input
;   Outputs: Console - evaluation of expression or error
;	     R5 - evaluation of expression
; 		 R1 - used as a temporary register
EVALUATE
ST R7, EVAL_SAVER7 ;save R7
Loop3 GETC				 ;get input character
OUT

;Load and add negated val of ASCII '=' to input
LD R1, EQUALS
ADD R1, R1, R0
BRz EVALUATED


;Load and add negated value ASCII ' ' to input
LD R1, SPACE			;
ADD R1, R1, R0
BRz Loop3


;Checks if '+' input
LD R1, ADDITION
ADD R1, R1, R0
BRnp #2
JSR PLUS
BRnzp Loop3

;Checks for '-' input
LD R1, SUBTRACTS
ADD R1, R1, R0
BRnp #2
JSR MIN
BRnzp Loop3

;Checks for '*' input
LD R1, MULTIPLIES
ADD R1, R1, R0
BRnp #2
JSR MUL
BRnzp Loop3

;Checks for '/' input
LD R1, DIVIDES
ADD R1, R1, R0
BRnp #2
JSR DIV
BRnzp Loop3

;Checks to make sure operand is greater than ascii value '0'
LD R1, LOWER
ADD R1, R1, R0
BRn UNDERFLOW

;Checks to make sure operand is less than ascii value '9'
LD R1, UPPER
ADD R1, R1, R0
BRp UNDERFLOW


LD R4, SHIFT
ADD R0, R4, R0

JSR PUSH
BRnzp Loop3

;comes here when there is an equals sign in the input
EVALUATED
JSR POP
;Checks if STACK_TOP and STACK_START are equal. If they are, then there is only one value. If not, there is more than one.
LD R6, STACK_TOP
LD R1, STACK_START
NOT R6, R6
ADD R6, R6, #1
ADD R6, R1, R6
BRnp UNDERFLOW

;Put evaluated value into R5
AND R5, R5, #0
ADD R5, R5, R0
LD R7, EVAL_SAVER7
RET

EVAL_SAVER7 .BLKW #1
UPPER .FILL xFFC7
LOWER .FILL xFFD0
EQUALS .FILL xFFC3
SUBTRACTS .FILL xFFD3
ADDITION .FILL xFFD5
MULTIPLIES .FILL xFFD6
DIVIDES .FILL xFFD1
SPACE .FILL xFFE0



; PLUS
;   Description: adds two numbers (R0 = R3 + R4)
;   Inputs: R3 - addend
;	    R4 - addend
;   Outputs: R0 - sum
PLUS
ST R7, PLUS_SAVER7
JSR POP
AND R4, R4, #0
ADD R4, R0, #0
JSR POP
AND R3, R3, #0
ADD R3, R0, #0

ADD R0, R3, R4;Well it's just adding isn't it
JSR PUSH
LD R7, PLUS_SAVER7
RET

SHIFT .FILL xFFD0
PLUS_SAVER7 .BLKW #1


; MIN
;   Description: subtracts two numbers (R0 = R3 - R4)
;   Inputs: R3 - minuend
;	    R4 - subtrahend
;   Outputs: R0 - difference
MIN
ST R7, MIN_SAVER7
JSR POP
AND R4, R4, #0
ADD R4, R0, #0
JSR POP
AND R3, R3, #0
ADD R3, R0, #0

NOT R4, R4
ADD R4, R4, #1
ADD R0, R4, R3; R4 + (-R3)
JSR PUSH
LD R7, MIN_SAVER7
RET

MIN_SAVER7 .BLKW #1

; MUL
;   Description: multiplies two numbers (R0 = R3 * R4)
;   Inputs: R3 - factor
;	    R4 - factor
;   Outputs: R0 - product
MUL
ST R7, MUL_SAVER7
JSR POP
AND R4, R4, #0
ADD R4, R0, #0
JSR POP
AND R3, R3, #0
ADD R3, R0, #0

AND R0, R0, #0; Clear R0
ADD R3, R3, #0; Set CC for R3
Loop BRz MULTIPLIED
;Terminate the multiplication once it is done
ADD R0, R0, R4;Add R4 to product as long as R3 is not zero
ADD R3, R3, #-1; Decrement the number of times R4 still needs to be added
BRnzp Loop
;Keep loop going
MULTIPLIED
JSR PUSH
LD R7, MUL_SAVER7
RET

MUL_SAVER7 .BLKW #1

; DIV
;   Description: divides two numbers (R0 = R3 / R4)
;   Inputs: R3 - numerator
;	    R4 - denominator
;   Outputs: R0 - quotient
DIV
ST R7, DIV_SAVER7
JSR POP
AND R4, R4, #0
ADD R4, R0, #0
JSR POP
AND R3, R3, #0
ADD R3, R0, #0

AND R0, R0, #0; Set quotient to 0
NOT R4, R4;
ADD R4, R4, #1;Negate R4 to make adding/subtracting simpler
Loop2 ADD R0, R0, #1; Count of the number of times loop traversed
ADD R3, R3, R4; Reduce numerator by denominator value
BRn DIVIDED ; Terminate loop once indication is found for division to be done
BRzp Loop2 ;Loop if not done with dividing
DIVIDED ADD R0, R0, #-1; Decrement quotient once to account for overcounting
JSR PUSH
LD R7, DIV_SAVER7
RET

DIV_SAVER7 .BLKW #1



; PUSH
;   Description: Pushes a character unto the stack
;   Inputs: R0 - character to push unto the stack
;   Outputs: R5 - 0 (success) or 1 (failure/overflow)
;   Registers: R3 - stores STACK_END
;	       R4 - stores STACK_TOP
PUSH
	ST R3, PUSH_SaveR3	;save R3
	ST R4, PUSH_SaveR4	;save R4
	AND R5, R5, #0		;
	LD R3, STACK_END	;
	LD R4, STACK_TOP	;
	ADD R3, R3, #-1		;
	NOT R3, R3		;
	ADD R3, R3, #1		;
	ADD R3, R3, R4		;
	BRz OVERFLOW		;stack is full
	STR R0, R4, #0		;no overflow, store value in the stack
	ADD R4, R4, #-1		;move top of the stack
	ST R4, STACK_TOP	;store top of stack pointer
	BRnzp DONE_PUSH		;
OVERFLOW
	ADD R5, R5, #1		;
DONE_PUSH
	LD R3, PUSH_SaveR3	;
	LD R4, PUSH_SaveR4	;
	RET


PUSH_SaveR3	.BLKW #1	;
PUSH_SaveR4	.BLKW #1	;


; POP
;   Description: Pops a character off the stack
;   Inputs:
;   Outputs: R0 - character popped off the stack
;	     R5 - 0 (success) or 1 (failure/underflow)
;   Registers: R3 - stores STACK_END
;	       R4 - stores STACK_TOP

POP
	ST R7, POP_SAVER7
	ST R3, POP_SaveR3	;save R3
	ST R4, POP_SaveR4	;save R3
	AND R5, R5, #0		;clear R5
	LD R3, STACK_START	;
	LD R4, STACK_TOP	;
	NOT R3, R3		;
	ADD R3, R3, #1		;
	ADD R3, R3, R4		;
	BRz UNDERFLOW		;
	ADD R4, R4, #1		;
	LDR R0, R4, #0		;
	ST R4, STACK_TOP	;
	BRnzp DONE_POP		;
UNDERFLOW
	LEA R0, Warning
	PUTS; Prints warning string if underflow occurs
	ADD R5, R5, #1		;
	BRnzp DONE
DONE_POP
	LD R3, POP_SaveR3	;
	LD R4, POP_SaveR4	;
	LD R7, POP_SAVER7
	RET

POP_SAVER7 .BLKW #1
Warning .STRINGZ "Invalid Expression"; Message to be printed when underflow or leftover operands occur
POP_SaveR3	.BLKW #1	;
POP_SaveR4	.BLKW #1	;
STACK_END	.FILL x3FF0	;
STACK_START	.FILL x4000	;
STACK_TOP	.FILL x4000	;


.END
