;
;	 HRY_411_PROJ1 with timer
;
; 
;	Author : Dimitris Ntounetas
;	AM:2016030141


.include "m16def.inc"
	.def	LoopRl = r24		; loop register low Byte
	.def	LoopRh = r25		; loop register high Byte
	.equ	LoopValue = 1000	; Loops to count
	
	.cseg
	.org 0x00    
    rjmp Main		
	.org OC1Aaddr		;interrupt Handler address
	rjmp ResetInterrupts
	
Main:
	ldi r17,1				;(1 cycle)
	out	DDRB,r17			;(1 cycle) set PINB0 to output
		
		

	;Prescaler 1 Initialisation 
	ldi	r16, (1<<CS10)|(1<<CS12)|(1<<WGM12)  ; Load immediate bits CS10 and WGM12 into TCCR1B Register (1 cycle)
	out	TCCR1B, r16     ;(1 cycle)

	;enable Compare Interrupt
	ldi r16, (1<<OCIE1A) ;Load immediate bit OCIE1A into TIMSK Register (1 cycle)
	out TIMSK, r16      ;(1 cycle)

	ldi LoopRh,HIGH(LoopValue)       ; higher 8 bits (1 cycle)
	ldi LoopRl,LOW(LoopValue)       ; lower 8 bits (1 cycle)
	out OCR1AH, LoopRh   ; Load 8 higher bytes into Compare Register 1(1 cycle)
	out OCR1AL, LoopRl   ;	Load 8 lower bytes into Compare Register 1(1 cycle)


	ldi r16,HIGH(RAMEND)
	out SPH,r16
	ldi r16,LOW(RAMEND)
	out SPL,r16
	sei ;enable interupts

	;infite loop
	label:
	rjmp label
	
ResetInterrupts: 
	
	sbic PORTB, 0
	jmp _clear_

	sbis PORTB, 0
	jmp _set_

	_set_:
			sbi PORTB, 0
			reti

	_clear_:
			cbi PORTB, 0
			reti
