.ORIG x3000
	LEA R0, #4		; Prompt user for input
	PUTS
	GETC					; Get input from user
	ADD R0, R0, #-16
	ADD R0, R0, #-16
	ADD R0, R0, #-16
	ADD R0, R0, #0
	BRnz INVALID ;
	ADD R0, R0, #-6
	BRp INVALID       ;
	OUT
LOOP				; Go thR0ugh the days of the week
	LEA R0, #3
	ADD R0, R0, #0
	BRzp DISPLAY				; Day of week found, so display
	ADD R0, R0, #10		; Go to the next day of the week
	ADD R0, R0, #-1		; Decrement the counter
	LD R0, #-254
	BRnp LOOP;
	ST R0, #4;
	LDR R0, R1, #3		;
	STR R1, R2, #4
	NOT R1, R2
	STI R0, #4
	LDI R2, #2 ; RTI
	RET;
DISPLAY				; Display the day of the week
	PUTS
INVALID				; Input is invalid
HALT

VAL .FILL x235
VAL1 .STRINGZ "venkata chary, padala"

.END
