;
; The code given to you here implements the histogram calculation that
; we developed in class.  In programming lab, we will add code that
; prints a number in hexadecimal to the monitor.
;
; Your assignment for this program is to combine these two pieces of
; code to print the histogram to the monitor.
;
; If you finish your program,
;    ** commit a working version to your repository  **
;    ** (and make a note of the repository version)! **


	.ORIG	x3000		; starting address is x3000


;
; Count the occurrences of each letter (A to Z) in an ASCII string
; terminated by a NUL character.  Lower case and upper case should
; be counted together, and a count also kept of all non-alphabetic
; characters (not counting the terminal NUL).
;
; The string starts at x4000.
;
; The resulting histogram (which will NOT be initialized in advance)
; should be stored starting at x3F00, with the non-alphabetic count
; at x3F00, and the count for each letter in x3F01 (A) through x3F1A (Z).
;
; table of register use in this part of the code
;    R0 holds a pointer to the histogram (x3F00)
;    R1 holds a pointer to the current position in the string
;       and as the loop count during histogram initialization
;    R2 holds the current character being counted
;       and is also used to point to the histogram entry
;    R3 holds the additive inverse of ASCII '@' (xFFC0)
;    R4 holds the difference between ASCII '@' and 'Z' (xFFE6)
;    R5 holds the difference between ASCII '@' and '`' (xFFE0)
;    R6 is used as a temporary register
;

	LD R0,HIST_ADDR      	; point R0 to the start of the histogram

	; fill the histogram with zeroes
	AND R6,R6,#0		; put a zero into R6
	LD R1,NUM_BINS		; initialize loop count to 27
	ADD R2,R0,#0		; copy start of histogram into R2

	; loop to fill histogram starts here
HFLOOP	STR R6,R2,#0		; write a zero into histogram
	ADD R2,R2,#1		; point to next histogram entry
	ADD R1,R1,#-1		; decrement loop count
	BRp HFLOOP		; continue until loop count reaches zero

	; initialize R1, R3, R4, and R5 from memory
	LD R3,NEG_AT		; set R3 to additive inverse of ASCII '@'
	LD R4,AT_MIN_Z		; set R4 to difference between ASCII '@' and 'Z'
	LD R5,AT_MIN_BQ		; set R5 to difference between ASCII '@' and '`'
	LD R1,STR_START		; point R1 to start of string

	; the counting loop starts here
COUNTLOOP
	LDR R2,R1,#0		; read the next character from the string
	BRz PRINT_HIST		; found the end of the string

	ADD R2,R2,R3		; subtract '@' from the character
	BRp AT_LEAST_A		; branch if > '@', i.e., >= 'A'
NON_ALPHA
	LDR R6,R0,#0		; load the non-alpha count
	ADD R6,R6,#1		; add one to it
	STR R6,R0,#0		; store the new non-alpha count
	BRnzp GET_NEXT		; branch to end of conditional structure
AT_LEAST_A
	ADD R6,R2,R4		; compare with 'Z'
	BRp MORE_THAN_Z         ; branch if > 'Z'

; note that we no longer need the current character
; so we can reuse R2 for the pointer to the correct
; histogram entry for incrementing
ALPHA	ADD R2,R2,R0		; point to correct histogram entry
	LDR R6,R2,#0		; load the count
	ADD R6,R6,#1		; add one to it
	STR R6,R2,#0		; store the new count
	BRnzp GET_NEXT		; branch to end of conditional structure

; subtracting as below yields the original character minus '`'
MORE_THAN_Z
	ADD R2,R2,R5		; subtract '`' - '@' from the character
	BRnz NON_ALPHA		; if <= '`', i.e., < 'a', go increment non-alpha
	ADD R6,R2,R4		; compare with 'z'
	BRnz ALPHA		; if <= 'z', go increment alpha count
	BRnzp NON_ALPHA		; otherwise, go increment non-alpha

GET_NEXT
	ADD R1,R1,#1		; point to next character in string
	BRnzp COUNTLOOP		; go to start of counting loop



PRINT_HIST
;INTRO PARAGRAPH: The entire program creates and prints a histogram for the characters frequencies provided.
;Using the existing the code from lab 1 with other code we wrote, we were able to print the histogram in
;the desired format.
;partners: sap3, abhaysp2

; table of register use in this part of the code
;    R0 is used a temporary register and to OUT to display
;    R1 holds a pointer to the starting address of the histogram
;    R2 holds the ASCII Value for the label of the bins(i.e. @ A B C ....)
;    R3 is used to hold the frequency count for each bin that was in the histogram
;    (starting at address HIST_ADDR)
;
;    R6 is a counter for the bins(27 bins in total)

			LD R1, HIST_ADDR ;load pointer with starting address for histogram
			LD R2, START	 ;load label for bins starting with @
			LD R6, NUM_BINS   ;set bin counter 27

LOOP	    LDR R3, R1, 0	;Load the frequency value stored in the histogram address

			AND R0, R0, 0	;prints the label for each bin
			ADD R0, R2, 0
			OUT

			LD R0, SPACE 	;prints space chara
			OUT

;save all register previous vals
			ST R1, SAVE_R1
			ST R2, SAVE_R2
			ST R6, SAVE_R6

; table of register use in this part of the code
;    R0 is used as a temporary register and to OUT to display
;    R1 is the digit counter (4 digits)
;    R2 is the bit counter
;    R3 holds the frequency count for the character that is converted to hex
;	 R4 holds the digit
;    R5 is used as a temporary register
;    R6 is used as a temporary register

;LAB PARAGRAPH: This part of the program comes from the lab in which we print the hexadecimal
;representation of a register's content using ASCII value conversions.

            AND R1, R1, #0 ;initialize digit counter
            ADD R1, R1, #4 ;set digit counter to 4

NEXT_DIGIT  AND R6, R6, #0
            ADD R1, R1, #0
            BRz NEW_LINE

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
JUMP        AND R0, R0, 0
						ADD R0, R4, #0
            OUT

            ADD R1, R1, #-1 ;decrements digit counter
            BRnzp NEXT_DIGIT





NEW_LINE	LD R0, ENDLINE ;prints new line
			OUT

;restore all register previous values
			LD R1, SAVE_R1
			LD R2, SAVE_R2
			LD R6, SAVE_R6


			ADD R1, R1, #1 ;increment histogram pointer
			ADD R2, R2, #1 ;increment character label
			ADD R6, R6, #-1 ;decrement bin counter
			BRp LOOP



DONE		HALT			; done


; the data needed by the program
NUM_BINS	.FILL #27	; 27 loop iterations
NEG_AT		.FILL xFFC0	; the additive inverse of ASCII '@'
AT_MIN_Z	.FILL xFFE6	; the difference between ASCII '@' and 'Z'
AT_MIN_BQ	.FILL xFFE0	; the difference between ASCII '@' and '`'
HIST_ADDR	.FILL x3F00 ; histogram starting address
STR_START	.FILL x4000	; string starting address
START		.FILL x0040 ; Start of Bin Chars
SPACE		.FILL x0020 ; Space Char
ENDLINE 	.FILL x0A	; new line

SAVE_R2		.BLKW #1
SAVE_R1  	.BLKW #1
SAVE_R6		.BLKW #1


ZERO       	.FILL x0030	 ; ASCII value for 0
A         	.FILL X0041	 ; ASCII value for A


; for testing, you can use the lines below to include the string in this
; program...
; STR_START	.FILL STRING	; string starting address
; STRING		.STRINGZ "This is a test of the counting frequency code.  AbCd...WxYz."



	; the directive below tells the assembler that the program is done
	; (so do not write any code below it!)

	.END
