lea r2, neg97   ; Load address of neg97 into r2.
ldr r2, r2, #0  ; Load contents of address in r2, into r2 (r2 now holds -97).

input:              ; Read input.
  in                ; Read one character into r0[7:0]
  add r5, r0, #-10  ; Subtract 10, because enter key has value 10.
  BRz output        ; If zero (enter pressed), we're done so branch to output.

  lea r4, array     ; Load starting address of array into r4.
  add r0, r0, r2    ; Subtract 97 to find ASCII index.
  BRz ltrcnt











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

  VAL .FILL 0x235

  .END
