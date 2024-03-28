/*
 * HRY_411_Project8.c
 *
 * Created: 5/12/2020
 * Dimitris Ntounetas
 * AM:2016030141
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include "Declarations.h"
// Pointer Creations

// Start of SRAM Address for the Answer
volatile uint8_t *UARTAnsAddress = (uint8_t *)0x0090;
// Start of SRAM Address for the Instruction
volatile uint8_t *InstrAddress = (uint8_t *)0x00A0;
// RingCounter Left Address
volatile uint8_t *RingCounterL = (uint8_t *)0x00B0;
// RingCounter Right Address
volatile uint8_t *RingCounterR = (uint8_t *)0x00B1;
// Led Toggle Address
volatile uint8_t *LedToggle = (uint8_t *)0x00B2;
// TimeSlice END Address
volatile uint8_t *TimeSliceEND = (uint8_t *)0x00B3;
// Current Process
volatile uint8_t *CurrProcess = (uint8_t *)0x00B4;
// Process 1 active
volatile uint8_t *Proc1Active = (uint8_t *)0x00B5;
// Process 2 active
volatile uint8_t *Proc2Active = (uint8_t *)0x00B6;
// Process 3 active
volatile uint8_t *Proc3Active = (uint8_t *)0x00B7;

void sendAns(){
	// 	enable UDRIE Flag
	UCSRB = (1<<UDRIE)|(1<<RXEN)|(1<<TXEN)|(1<<RXCIE)|(1<<TXCIE);
}
void ProcStart(){
	InstrAddress +=1;
	switch(*InstrAddress){
		case ASCII_1:
		*Proc1Active = 1;
		break;
		case ASCII_2:
		*Proc2Active = 1;
		break;
		case ASCII_3:
		*Proc3Active = 1;
		break;
	}
	sendAns();
	InstrAddress = (uint8_t *)0x00A0;
	
}
void ProcQuit(){
	InstrAddress +=1;
	switch(*InstrAddress){
		case ASCII_1:
		*Proc1Active = 0;
		break;
		case ASCII_2:
		*Proc2Active = 0;
		break;
		case ASCII_3:
		*Proc3Active = 0;
		break;
	}
	sendAns();
	InstrAddress = (uint8_t *)0x00A0;
}
void CTC_Timer_Interrupt(){
	
	switch(*CurrProcess){
		case 1:
		if (*Proc2Active == 1)
		{
			*CurrProcess = 2;
		}
		else if (*Proc3Active)
		{
			*CurrProcess = 3;
		}
		else if(*Proc1Active)
		{
			*CurrProcess = 1;
		}
		else
		{
			*CurrProcess = 0;
		}
		break;
		case 2:
		if (*Proc3Active == 1)
		{
			*CurrProcess = 3;
		}
		else if(*Proc1Active)
		{
			*CurrProcess = 1;
		}
		else if(*Proc2Active)
		{
			*CurrProcess = 2;
		}
		else
		{
			*CurrProcess = 0;
		}
		break;
		case 3:
		if (*Proc1Active == 1)
		{
			*CurrProcess = 1;
		}
		else if(*Proc2Active)
		{
			*CurrProcess = 2;
		}
		else if(*Proc3Active)
		{
			*CurrProcess = 3;
		}
		else
		{
			*CurrProcess = 0;
		}
		break;
		default:
		if (*Proc1Active == 1)
		{
			*CurrProcess = 1;
		}
		else if(*Proc2Active)
		{
			*CurrProcess = 2;
		}
		else if(*Proc3Active)
		{
			*CurrProcess = 3;
		}
		else
		{
			*CurrProcess = 0;
		}
		break;
		
	}
	*TimeSliceEND = 1;
	return;
}
void RXC_Interrupt(){
	register uint8_t temp asm("r16");
	// 	read character from UDR and from r16 register
	// read from UDR 2 times to consume UDRE flag
	*InstrAddress = UDR;
	*InstrAddress = UDR;
	*InstrAddress = temp;
	// 	if character is LF it means we have the last char of the instruction
	// so we check which instruction we have
	if(*InstrAddress == ASCII_LF){
		InstrAddress = (uint8_t *)0x00A0;
		switch(*InstrAddress){
			case ASCII_S:
			ProcStart();
			break;
			case ASCII_Q:
			ProcQuit();
			break;
			default:
			//error wrong instruction
			
			break;
		}
	}
	else //or move to next char
	InstrAddress += 1;
	
}
void UDRE_Interrupt(){
	// 	send to UDR next character from answer
	UDR = *UARTAnsAddress;
	UDR = *UARTAnsAddress;
	TCNT2 = *UARTAnsAddress;
	
	// 	if character sent is LF we have sent the answer
	if (*UARTAnsAddress == ASCII_LF)
	{
		// 		disable UDRIE interrupt
		UARTAnsAddress = (uint8_t *)0x0090;
		UCSRB = (1<<RXEN)|(1<<TXEN)|(1<<RXCIE)|(1<<TXCIE);
	}else //go to next character
	UARTAnsAddress += 1;
}
void UTXC_Interrupt(){
	InstrAddress = (uint8_t *)0x00A0;
	uint8_t i;
	// 	clear instruction so we are ready for the next
	for (i=0;i<=4;i++){
		*InstrAddress = 0;
		InstrAddress +=1;
	}
	InstrAddress = (uint8_t *)0x00A0;
}
// Interrupt Service Routines
ISR(TIMER1_COMPA_vect)
{
	CTC_Timer_Interrupt();
}
ISR(USART_RXC_vect)
{
	RXC_Interrupt();
}
ISR(USART_UDRE_vect)
{
	UDRE_Interrupt();
}
ISR(USART_TXC_vect)
{
	UTXC_Interrupt();
}

void initUART(){

	UBRRL = LOW(UBRRValue);			// load baud prescale
	UBRRH = HIGH(UBRRValue);		// to UBRR

	UCSRB = (1<<RXEN)|(1<<TXEN)|(1<<RXCIE)|(1<<TXCIE); // enable transmitter, receiver
	UCSRC = (1<<URSEL)|(1<<UCSZ1)|(1<<UCSZ0);			//Set frame format: 8data, 1stop bit
}
void initPorts(){
	// 	Ports Initialization
	DDRA = 0xFF;	//Port A Initialization as output

	//	Enable Compare Interrupt
	TIMSK = (1<<OCIE1A);
	OCR1AH = HIGH(LoopValue);
	OCR1AL = LOW(LoopValue);
	TCCR1B = (1<<CS11)|(1<<CS10)|(1<<WGM12);
}
void initRam(){
	
	// UART answer Initialization
	*UARTAnsAddress = ASCII_O;
	UARTAnsAddress += 1;
	*UARTAnsAddress = ASCII_K;
	UARTAnsAddress += 1;
	*UARTAnsAddress = ASCII_CR;
	UARTAnsAddress += 1;
	*UARTAnsAddress = ASCII_LF;
	UARTAnsAddress += 1;
	UARTAnsAddress = (uint8_t *)0x0090;
	
	//	Instruction Address Initialization
	InstrAddress = (uint8_t *)0x00A0;

	// 	Left RingCounter init
	*RingCounterL = 0b00000001;

	// 	Right RingCounter init
	*RingCounterR = 0b10000000;

	// 	Led Toggle init
	*LedToggle = 0b01010101;

	//	TimeSliceEND init
	*TimeSliceEND = 0;
	//	Current Process
	*CurrProcess = 0;
	//	Process1 Active
	*Proc1Active = 0;
	//	Process2 Active
	*Proc2Active = 0;
	//	Process3 Active
	*Proc3Active = 0;
}
void initialisation(void){
	
	//Chip Initializations
	initUART();
	initPorts();
	initRam();
	sei();
}

void Proc1(){
	
	// 	Ringcounter Left
	if (*RingCounterL == 0b00000000)
	{
		*RingCounterL = 0b00000001;
		}else{
		*RingCounterL = *RingCounterL << 1;
	}
	
	PORTA = *RingCounterL;
	
	// Return is not necessary because Compiler adds- return in disassembly
}

void Proc2(){
	
	// 	Ringcounter Right
	if (*RingCounterR == 0b00000000)
	{
		*RingCounterR = 0b10000000;
		}else{
		*RingCounterR = *RingCounterR >> 1;
	}
	PORTA = *RingCounterR;
	
	// Return is not necessary because Compiler adds- return in disassembly
}

void Proc3(){
	
	*LedToggle = *LedToggle ^ 0b11111111;
	
	PORTA = *LedToggle;
	
	*TimeSliceEND = 0;
	// Return is not necessary because Compiler adds- return in disassembly
}
int main(void)
{
	initialisation();
	while (1)
	{
		switch(*CurrProcess){
			case 1:
			
			while(*TimeSliceEND == 0){
				
				Proc1();
				
			}
			*TimeSliceEND = 0;

			break;
			case 2:
			
			while(*TimeSliceEND == 0)
			{
				Proc2();
			}
			*TimeSliceEND = 0;
			
			break;
			case 3:
			
			while(*TimeSliceEND == 0)
			{
				Proc3();
			}
			*TimeSliceEND = 0;
			
			break;
			default:
			CTC_Timer_Interrupt();
			break;
			
		}
	}
}
