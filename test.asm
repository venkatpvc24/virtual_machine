.ORIG x3000
	ADD R0, R0, #-16
	ADD R0, R0, #-16
	ADD R0, R0, #-16

; Validate input
;/-----------------------------

;check lower bound and check upper bound
;	if input < 0
;		invalid
;	else if input - 6 > 0
;		invalid
;	else
;		valid
;/-----------------------------
	ADD R0, R0, #0
	BRn INVALID
	ADD R0, R0, #-6
	BRp INVALID
;	OUT
; Display the day of the week
;/-----------------------------

;/-----------------------------

LOOP				; Go thR0ugh the days of the week
	LEA R0, DAYS
	ADD R0, R0, #0
	BRz DISPLAY		; Day of week found, so display
	ADD R0, R0, #10		; Go to the next day of the week
	ADD R0, R0, #-1		; Decrement the counter
	BRp LOOP
DISPLAY				; Display the day of the week
	PUTS

INVALID				; Input is invalid

HALT

PROMPT .STRINGZ "Please enter a number fR0m 0-6: "
DAYS .STRINGZ "Sunday   "
 .STRINGZ "Monday   "
 .STRINGZ "Tuesday  "
 .STRINGZ "Wendsday "
 .STRINGZ "Thursday "
 .STRINGZ "Friday   "
 .STRINGZ "Saturday "


.END
