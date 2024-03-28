/*
 * HRY_411_Project7.c
 *
 * Created: 26/11/2020 
 * Ntounetas Dimitris
 * AM: 2016030141
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
	
	*CurrProcess += 1;
	if (*CurrProcess == 4)
	{
		*CurrProcess = 1;
	}
	*TimeSliceEND = 1;
	return;
}
void RXC_Interrupt(){
	
}
void UDRE_Interrupt(){
	
}
void UTXC_Interrupt(){
	
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
	*Proc1Active = 1;
//	Process2 Active
	*Proc2Active = 1;
//	Process3 Active
	*Proc3Active = 1;
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
