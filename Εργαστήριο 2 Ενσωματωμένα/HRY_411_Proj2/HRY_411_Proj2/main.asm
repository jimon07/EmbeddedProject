;
;	 HRY_411_PROJ2 BCD 7 segment display
;
;	Author : Dimitris Ntounetas
;	AM : 2016030141

	.include "m16def.inc"
	.def	temp = r16
	.def	RingCounter = r17
	.def	digitReg = r18
	.def	Initialized = r19
	.def	Seg_data_out = r20
	.def	digitCounter = r21
	.def	LoopRl = r24		; loop register low Byte
	.def	LoopRh = r25		; loop register high Byte
	.equ	LoopValue = 10000	; Loops to count
	
	; 7segment values for Display of Digits
	.equ	seg_zero = 0b11000000
	.equ	seg_one = 0b11111001
	.equ	seg_two = 0b10100100
	.equ	seg_three = 0b10110000
	.equ	seg_four = 0b10011001
	.equ	seg_five = 0b10010010
	.equ	seg_six = 0b10000010
	.equ	seg_seven = 0b11111000
	.equ	seg_eight = 0b10000000
	.equ	seg_nine = 0b10011000

	;Start of SRAM Address for the 8 Digits
	
	.equ	display_address = 0x0060
	
	.cseg
		
	.org 0x00
    
    rjmp Main		

;interrupt Handler address	
	.org OC1Aaddr		

	;Loop the same 8 digits	
		cpi digitCounter , 8
		breq Reset_digit_Counter

End_Reset_digit_Counter:
	;load From Memory next byte 
		ld digitReg, Y+
		inc digitCounter
		rcall BCD_TO_7_seg
		
		ldi temp,0XFF
		out PORTA,temp
		out PORTC,RingCounter
		out PORTA,Seg_data_out

		rol RingCounter
		
		reti
	
Main:
	
		cpi Initialized ,1
		breq label
	;RAM Initialisation
		ldi temp , 0
		sts display_address,temp
		ldi temp , 1
		sts display_address+1,temp
		ldi temp , 2
		sts display_address+2,temp
		ldi temp , 3
		sts display_address+3,temp
		ldi temp , 4
		sts display_address+4,temp
		ldi temp , 5
		sts display_address+5,temp
		ldi temp , 6
		sts display_address+6,temp
		ldi temp , 7
		sts display_address+7,temp

	;Énitialize Y pointer
		ldi	YL,LOW(display_address)		
		ldi	YH,HIGH(display_address)	

		clr digitCounter
	;Ports Initialisation

		ldi temp,0b11111111
		out DDRA,temp		;Port A Initialisation as output
		out DDRC,temp		;Port C Initialisation as output

		ldi RingCounter,0b00000001		;Ring counter init

	;enable Compare Interrupt
		ldi temp, (1<<OCIE1A) ;Load immediate bit OCIE1A into TIMSK Register (1 cycle)
		out TIMSK, temp      ;(1 cycle)

		ldi LoopRh,HIGH(LoopValue)       ; higher 8 bits (1 cycle)
		ldi LoopRl,LOW(LoopValue)       ; lower 8 bits (1 cycle)
		out OCR1AH, LoopRh   ; Load 8 higher bytes into Compare Register 1(1 cycle)
		out OCR1AL, LoopRl   ;	Load 8 lower bytes into Compare Register 1(1 cycle)

	;Prescaler 1 Initialisation 
		ldi	temp, (1<<CS10)|(1<<WGM12)  ; Load immediate bits CS10 and WGM12 into TCCR1B Register (1 cycle)
		out	TCCR1B, temp     ;(1 cycle)

	;enable interupts
		sei 

		ldi Initialized ,1
	;infite loop
		label:
		rjmp label

Reset_digit_Counter:
		clr digitCounter
		ldi RingCounter,0b00000001
		clr Initialized 

		jmp End_Reset_digit_Counter

BCD_TO_7_seg:
		cpi digitReg,0
		breq ZeroDigit
		cpi digitReg,1
		breq OneDigit
		cpi digitReg,2
		breq TwoDigit
		cpi digitReg,3
		breq ThreeDigit
		cpi digitReg,4
		breq FourDigit
		cpi digitReg,5
		breq FiveDigit
		cpi digitReg,6
		breq SixDigit
		cpi digitReg,7
		breq SevenDigit
		cpi digitReg,8
		breq EightDigit
		cpi digitReg,9
		breq NineDigit

	ZeroDigit:
		ldi Seg_data_out,seg_zero
		ret
	OneDigit:
		ldi Seg_data_out,seg_one
		ret
	TwoDigit:
		ldi Seg_data_out,seg_two
		ret
	ThreeDigit:
		ldi Seg_data_out,seg_three
		ret
	FourDigit:
		ldi Seg_data_out,seg_four
		ret
	FiveDigit:
	 	ldi Seg_data_out,seg_five
		ret
	SixDigit:
		ldi Seg_data_out,seg_six
		ret
	SevenDigit:
		ldi Seg_data_out,seg_seven
		ret
	EightDigit:
		ldi Seg_data_out,seg_eight
		ret
	NineDigit:
		ldi Seg_data_out,seg_nine
		ret