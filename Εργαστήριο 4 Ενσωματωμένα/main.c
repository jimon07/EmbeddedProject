/*
 * HRY_411_Proj4.c
 *
 * Created: 30/10/2020 1:34:35 μμ
 * Author : jim_1
 */ 
		#include <avr/io.h>
		#include <avr/interrupt.h>
		#include "Declarations.h"
		
	//Pointer Creations
		// Start of SRAM Address for the 8 Digits
		volatile uint8_t *display_address = (uint8_t *)0x0070;
		// Start of SRAM Address for the Answer
		volatile uint8_t *UARTAnsAddress = (uint8_t *)0x0080;
		// Start of SRAM Address for the Instruction
		volatile uint8_t *InstrAddress = (uint8_t *)0x0090;
		// RingCounter
		volatile uint8_t *RingCounterAddress = (uint8_t *)0x0100;
		// DigitReg
		volatile uint8_t *DigitRegAddress = (uint8_t *)0x0101;
		// DigitReg
		volatile uint8_t *DigitCounterAdress = (uint8_t *)0x0102;
		
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
	DDRC = 0xFF;	//Port C Initialization as output	

	//	Enable Compare Interrupt
	TIMSK = (1<<OCIE1A);
	OCR1AH = HIGH(LoopValue);
	OCR1AL = LOW(LoopValue);
	TCCR1B = (1<<CS10)|(1<<WGM12);
}
void initRam(){
	// RAM Initialization for display
	uint8_t i;
	for(i=0;i<=7;i++){
		
		*display_address = i;
		display_address += 1;
		}
	display_address = 0x0070;
	// UART answer Initialization
		*UARTAnsAddress = ASCII_O;
		UARTAnsAddress += 1;
		*UARTAnsAddress = ASCII_K;
		UARTAnsAddress += 1;
		*UARTAnsAddress = ASCII_CR;
		UARTAnsAddress += 1;
		*UARTAnsAddress = ASCII_LF;
		UARTAnsAddress += 1;
		
		UARTAnsAddress = 0x0080;
		
		//Instruction Address Initialization
		InstrAddress = 0x0090;
		
		//Counters Initializations
		RingCounterAddress = 0x0100;
		*RingCounterAddress = 0b00000001;
		
		DigitRegAddress = 0x0101;
		DigitCounterAdress = 0x0102;
}
void initialisation(void){
	//Chip Initializations	
		initUART();
		initRam();
		initPorts();
	// 	Enable interrupts
		sei();
}

int main(void)
{
    initialisation();
    while (1) 
    {
		
    }
}



