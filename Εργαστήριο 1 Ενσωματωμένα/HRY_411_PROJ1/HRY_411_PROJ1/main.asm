;
; HRY_411_PROJ1.asm
;
; Author : Ntounetas Dimitris
; AM:2016030141


; Replace with your application code
.include "m16def.inc"
	
	.def	ledR 	= r16		; led register
	.def	LoopRl = r24		; inner loop register low
	.def	LoopRh = r25		; inner loop register high

	.equ	iVal 	= 2499	; inner loop value
	
	.cseg
	.org	0x00
	
	ldi r17,1				;(1 cycle)
	out	DDRB,r17			;(1 cycle) set PINB0 to output
	
start:	
	clr	ledR			; (1 cycle) clear led register
	out	PORTB,ledR		; (1 cycle) write led register to PORTB
	ldi	LoopRl,LOW(iVal)	; (1 cycle) intialize Low 8 bits
	ldi	LoopRh,HIGH(iVal)	; (1 cycle) intialize High 8 bits

	;Loop takes 4 cycles. In order to delay for 1ms we have to consume 10000 cycles. 10000 cycles / 4 = 2500 loops.
	Loop:	sbiw	LoopRl,1		; decrement inner loop registers  (2 cycles)
	brne	Loop					; branch to iLoop if iLoop registers != 0	(2 cycles)


	ldi ledR,1				; led logical1
	out	PORTB,ledR			; write led register to PORTB
	rjmp	start			; jump back to start