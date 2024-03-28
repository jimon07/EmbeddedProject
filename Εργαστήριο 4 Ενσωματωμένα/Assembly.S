#include <avr/io.h>
#include "Declarations.h"

#define __SFR_OFFSET 0
#define display_address_pointer 0x006A
#define	display_address 0x0070
#define display_address_END 0x0078
#define	UARTAnsAddress_pointer 0x0068
#define	UARTAnsAddress 0x0080
#define	InstrAddress 0x0090
#define InstrAddress_END 0x009B
#define InstrAddress_pointer 0x0066
#define RingCounter_addr 0x0100
#define DigitReg_addr 0x0101
#define DigitCounter_addr 0x0102


.global CTC_Timer_Interrupt
.global RXC_Interrupt
.global UDRE_Interrupt
.global UTXC_Interrupt

//Loop the same 8 digits	
CTC_Timer_Interrupt:
		
		// Load Ring_Counter value
		ldi	ZL,LOW(RingCounter_addr)		
		ldi	ZH,HIGH(RingCounter_addr)
		ld temp,Z

		cpi temp, 0b00000000
		breq Reset_digit_Counter
		jmp Show_next_Digit
		
// Reset Digit counter for sync
Reset_digit_Counter:
		ldi	ZL,LOW(DigitCounter_addr)		
		ldi	ZH,HIGH(DigitCounter_addr)
		ldi temp,0						//Clear Digit Counter
		st Z,temp				
		ldi	ZL,LOW(RingCounter_addr)		
		ldi	ZH,HIGH(RingCounter_addr)
		ldi temp,0b00000001				//Reset Ring Counter
		st Z,temp
		LDI temp,display_address			//Load immediate   
		STS display_address_pointer,temp 	//Store direct to data space 
		jmp Show_next_Digit

Show_next_Digit:
	//load From Memory next byte 
		ldi	ZL,LOW(DigitCounter_addr)		
		ldi	ZH,HIGH(DigitCounter_addr)
		ld temp1,Z
		LDS temp,display_address_pointer		//Load direct from data space 
		mov ZH,r1
		mov ZL,temp
		
		ld temp1,Z
		
		rcall BCD_TO_7_seg
		
		ldi temp,0XFF
		out PORTA,temp

		ldi	ZL,LOW(RingCounter_addr)		
		ldi	ZH,HIGH(RingCounter_addr)
		ld temp,Z
		out PORTC,temp

		out PORTA,Seg_data_out

		rol temp

		st Z,temp

		ldi	ZL,LOW(DigitCounter_addr)		
		ldi	ZH,HIGH(DigitCounter_addr)
		ld temp,Z
		inc temp
		st Z,temp

		//Move display pointer 1 position 
		LDS temp,display_address_pointer		//Load direct from data space 
		SUBI temp,0xFF							//Subtract immediate 	 
		STS display_address_pointer,temp		//Store direct to data space 

		
		reti

		//;BCD to 7seg decoder
BCD_TO_7_seg:
		cpi temp1,0
		breq ZeroDigit
		cpi temp1,1
		breq OneDigit
		cpi temp1,2
		breq TwoDigit
		cpi temp1,3
		breq ThreeDigit
		cpi temp1,4
		breq FourDigit
		cpi temp1,5
		breq FiveDigit
		cpi temp1,6
		breq SixDigit
		cpi temp1,7
		breq SevenDigit
		cpi temp1,8
		breq EightDigit
		cpi temp1,9
		breq NineDigit
		cpi temp1,0x0A
		breq ClearDigit

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
	ClearDigit:
		ldi Seg_data_out,seg_clear
		ret


RXC_Interrupt:
	rcall getc
		reti

getc: 
	/*; For polling
	; in temp,UCSRA
	; sbrs temp, RXC
	; rjmp getc*/
	 LDS temp,InstrAddress_pointer		//Load direct from data space 
	 mov ZH,r1
	 mov ZL,temp

	 in temp, UDR
	 in temp, UDR //; Get Data from USART

	 mov temp, r15 //; Added for testing puproses. R15 is the input now
	 st Z,temp
	 //;Store Instruction into SRAM
		ldi temp1,ASCII_LF //; If last character was <LF>
		cpse temp , temp1 //; Instruction is all stored and go to InstructionDone
		rjmp .+4
		rcall InstructionDone
		ret
		//Move Instruction pointer 1 position
		LDS temp,InstrAddress_pointer		//Load direct from data space 
		SUBI temp,0xFF							//Subtract immediate 	 
		STS 0x0066,temp		//Store direct to data space 
		ret

//; USART Data Register Empty		
UDRE_Interrupt:		
		rcall putc
		reti
//; USART, Tx Complete
UTXC_Interrupt:	
		rcall AnswerSent
		reti

	putc: 
	//;Polling checks
	//;in temp, UCSRA 
	//;sbrs temp, UDRE
	//;rjmp putc
	lds temp, UARTAnsAddress_pointer
	mov ZH,r1
	mov ZL,temp
	// Move Pointer 1 position
	ld AnsReg, Z //;Load next character from Answer Address 
	SUBI temp,0xFF							//Subtract immediate 	 
	STS UARTAnsAddress_pointer,temp		//Store direct to data space 

	out UDR, AnsReg
	out UDR, AnsReg	//; Transmit to USART
	out TCNT2, AnsReg //; Send to TCNT2 for testing purposes.

	ldi temp,ASCII_LF	//;If current character is <LF>
	cpse AnsReg,temp	//; Answer is sent
	rjmp .+4		//; else return and wait for next character

	ldi	temp,(1<<RXEN)|(1<<TXEN)|(1<<RXCIE)|(1<<TXCIE)	; Disable UDRIE Cause Answer sent
	out	UCSRB,temp
	ret	

//; After the complete instruction is stored send Instruction Pointer at the beggining		
InstructionDone:
		//;Reinitialize X pointer
		LDS temp,InstrAddress_pointer		//Load direct from data space 

		ldi temp,0x0090;	// Go back to beggining	 
		STS InstrAddress_pointer,temp		//Store direct to data space 
		jmp CheckInstruction
//; Send OK<CR><LF> Answer
SendOK:
		ldi	temp,(1<<UDRIE)|(1<<RXEN)|(1<<TXEN)|(1<<RXCIE)|(1<<TXCIE)	//; enable transmitter and receiver
		out	UCSRB,temp	
		reti
//; Send programm to main function after answer sent
AnswerSent:
		//;Reinitialize Z pointer
		LDI temp,UARTAnsAddress		//Load immediate
		STS UARTAnsAddress_pointer,temp		//Store direct to data space 
		LDS temp,InstrAddress_pointer		//Load direct from data space 
		mov ZH , r1
		mov ZL , temp
		ldi temp1,0x9B
		rcall ResetInstrBuffer
		ret

ResetInstrBuffer:
//;Delete data from Instruction Buffer after Answer sent
		ldi temp,0
		st Z+,temp
		cpse ZL,temp1
		jmp ResetInstrBuffer
		ret
//;Check which instruction is Received
CheckInstruction:
		LDS temp,InstrAddress_pointer		//Load direct from data space 
		mov ZL,temp
		ld temp1, Z
		ldi temp,ASCII_A
		cpse temp1,temp //;If first Character is ASCII A
		rjmp .+2
		jmp ATInstr		//; Go to AT instruction
		ldi temp,ASCII_C 
		cpse temp1,temp //;If first Character is ASCII C
		rjmp .+2
		jmp CInstr //; Go to Clear instruction
		ldi temp,ASCII_N
		cpse temp1,temp //;If first Character is ASCII N
		rjmp .+2
		jmp NInstr  //; Go to Number instruction
		jmp Error	//;Else go to Error

//;Attention instruction
ATInstr:
		jmp SendOK
//;Clear instruction
CInstr:
		rcall StartClear
		jmp SendOK
//;Number instruction
NInstr:
		rcall StartClear
		ldi XL,LOW(InstrAddress_END)
		ldi	YL,LOW(display_address)		
		ldi	YH,HIGH(display_address)
		rcall RenewSeg
		jmp SendOK
//;Renew Segments
RenewSeg:
		ld temp,-X //;Load character from the end 
		ldi temp1,ASCII_CR //; until you find CR loop
		cpse temp,temp1
		jmp RenewSeg
		LoopRen:	
		ld temp,-X	//;Load number from the end
		ldi temp1,ASCII_N	//;Until character N
		cpse temp,temp1
		jmp .+2
		ret
		rcall AddNumToSeg //;call routine that stores number to segment address
		jmp LoopRen
//;Add number to a segment
AddNumToSeg:
		ld temp,X
		andi temp,0b00001111 //; Mask for clearing the upper 8 Bytes. Turn ASCII to BCD
		st Y+,temp	//;Store BCD for Segment
		ret
	
//;For Wrong input check to be implemented
Error:
		ret
//;Initiate clear of screen
StartClear:
		
		ldi temp,display_address;		// Go back to beggining	 
		STS display_address_pointer,temp	//Store direct to data space 

		ldi temp1,display_address_END	//;Until you reach the end of reserved memory
		LDS temp,display_address_pointer 
		mov ZL,temp	
		rcall clear7seg	//;call routine
		ret

//;Clear all screen
clear7seg:	
			
		ldi temp,0X0A //;Put 0x0A in every digit
		st Z+,temp	//;Store to displayAddress
		cpse ZL,temp1	//;Check if at the end of DisplayAddress
		jmp clear7seg
		jmp Reset_digit_Counter	//;Then Reset Screen
		ret