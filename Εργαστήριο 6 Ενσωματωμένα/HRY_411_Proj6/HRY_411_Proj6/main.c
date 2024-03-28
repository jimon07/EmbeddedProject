/*
 * HRY_411_Proj6.c
 *
 * Created: 18/11/2020
 * Author : Dimitris Ntounetas
 * AM: 2016030141
 */ 
		#include <avr/io.h>
		#include <avr/interrupt.h>
		#include "Declarations.h"
		#include <avr/wdt.h>
		
	//Pointer Creations
		// Start of SRAM Address for the 8 Digits
		volatile uint8_t *display_address = (uint8_t *)0x0080;
		// Start of SRAM Address for the Answer
		volatile uint8_t *UARTAnsAddress = (uint8_t *)0x0090;
		// Start of SRAM Address for the Instruction
		volatile uint8_t *InstrAddress = (uint8_t *)0x00A0;
		// Decode address
		volatile uint8_t *DecAddress = (uint8_t *)0x00B0;
		// Start of SRAM Address for the ResetAnswer
		volatile uint8_t *ResAddress = (uint8_t *)0x00C0;
		// Reset var
		volatile uint8_t *ResVar = (uint8_t *)0x00C3;
		// RingCounter
		volatile uint8_t *RingCounterAddress = (uint8_t *)0x0100;
		// DigitReg
		volatile uint8_t *DigitRegAddress = (uint8_t *)0x0101;
		// DigitReg
		volatile uint8_t *DigitCounterAdress = (uint8_t *)0x0102;

void CTC_Timer_Interrupt(){
// 	Reset when RingCounter = 0
	if (*RingCounterAddress == 0){
		*RingCounterAddress = 0b00000001;
		display_address = (uint8_t *)0x0080;
	}
// 	display output to ports
// 	first display nothing then display next
	PORTA = 0xff;
	PORTC = *RingCounterAddress;
// 	left swift ring counter
	*RingCounterAddress = *RingCounterAddress << 1;
	PORTA = *(DecAddress + *display_address);
	display_address += 1;
}
void sendAns(){
// 	enable UDRIE Flag
	UCSRB = (1<<UDRIE)|(1<<RXEN)|(1<<TXEN)|(1<<RXCIE)|(1<<TXCIE);
}
void clearseg(){
	uint8_t i;
// 	put 0x0A in every segment in display
	display_address = (uint8_t *)0x0080;
	for (i=0;i<=7;i++){
		*display_address=0x0A;
		display_address +=1;
	}
	display_address = (uint8_t *)0x080;
}
void NumberInstr(){
	clearseg();
// 	go to end of display address
	InstrAddress = (uint8_t *)0x00AB;
// 	go back until you find CR
	while (*InstrAddress != ASCII_CR){
		InstrAddress -=1;
	}
// 	go back one more
	InstrAddress -=1;
// 	put number from instruction to display in the opposite order
// 	left values in display are the smallest numbers in display
	while (*InstrAddress != ASCII_N){
		*display_address = *InstrAddress;
		display_address +=1;
		InstrAddress -=1;
	}
// 	return addresseses to the start
	InstrAddress = (uint8_t *)0x00A0;
	display_address = (uint8_t *)0x0080;
// 	send the answer
	sendAns();
}
void ClearInstr(){
// 	clear segments
	clearseg();
// 	send answer
	sendAns();
}
void AttentionInstr(){
// 	just send answer
	sendAns();
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
			case ASCII_N:
			NumberInstr();
			break;
			case ASCII_C:
			ClearInstr();
			break;
			case ASCII_A:
			AttentionInstr();
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
	if (*ResVar == 1){
		UDR = *ResAddress;
		UDR = *ResAddress;
		TCNT2 = *ResAddress;
	}else{
	UDR = *UARTAnsAddress;
	UDR = *UARTAnsAddress;
	TCNT2 = *UARTAnsAddress;
	}
// 	if character sent is LF we have sent the answer
	if (*UARTAnsAddress == ASCII_LF)
	{	
// 		disable UDRIE interrupt
		UARTAnsAddress = (uint8_t *)0x0090;
		UCSRB = (1<<RXEN)|(1<<TXEN)|(1<<RXCIE)|(1<<TXCIE);
	}
	else if (*ResAddress == ASCII_LF){
		ResAddress = (uint8_t *)0x00C0;
		*ResVar = 0;
		UCSRB = (1<<RXEN)|(1<<TXEN)|(1<<RXCIE)|(1<<TXCIE);
	}
	else if(*ResVar == 1){
		ResAddress += 1;
	}else //go to next character
	 UARTAnsAddress += 1;
}
void UTXC_Interrupt(){
	InstrAddress = (uint8_t *)0x00A0;
	uint8_t i;
// 	clear instruction so we are ready for the next
	for (i=0;i<=11;i++){
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
	wdt_reset();
	RXC_Interrupt();
}
ISR(USART_UDRE_vect)
{
	wdt_reset();
	UDRE_Interrupt();
}
ISR(USART_TXC_vect)
{
	wdt_reset();
	UTXC_Interrupt();
}

void initWDT(){
	// Initialize Watchdog Timer
	WDTCR = (1<<WDE)|(1<<WDP2);//|(1<<WDP1);//|(1<<WDP0);
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
	display_address = (uint8_t *)0x0080;
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
		
		//Instruction Address Initialization
		InstrAddress = (uint8_t *)0x00A0;
		
		//Decode Address Initialization
		//7seg 0
		*DecAddress = 0b11000000;
		DecAddress += 1;
		//7seg 1
		*DecAddress = 0b11111001;
		DecAddress += 1;
		//7seg 2
		*DecAddress = 0b10100100;
		DecAddress += 1;
		//7seg 3
		*DecAddress = 0b10110000;
		DecAddress += 1;
		//7seg 4
		*DecAddress = 0b10011001;
		DecAddress += 1;
		//7seg 5
		*DecAddress = 0b10010010;
		DecAddress += 1;
		//7seg 6
		*DecAddress = 0b10000010;
		DecAddress += 1;
		//7seg 7
		*DecAddress = 0b11111000;
		DecAddress += 1;
		//7seg 8
		*DecAddress = 0b10000000;
		DecAddress += 1;
		//7seg 9
		*DecAddress = 0b10011000;
		DecAddress += 1;
		//7seg clear
		*DecAddress = 0b11111111;
		DecAddress = (uint8_t *)0x00B0;
		
		*ResAddress = ASCII_R;
		ResAddress += 1;
		*ResAddress = ASCII_CR;
		ResAddress += 1;
		*ResAddress = ASCII_LF;
		ResAddress =(uint8_t *)0x00C0;
		
		//Counters Initializations
		RingCounterAddress = (uint8_t *)0x0100;
		*RingCounterAddress = 0b00000001;
		
		DigitRegAddress = (uint8_t *)0x0101;
		DigitCounterAdress = (uint8_t *)0x0102;
}
void initialisation(void){
	
	//Chip Initializations	
		initUART();
		
		initPorts();
		initWDT();
	//Enable interrupts
		sei();
	if (bit_is_set(MCUCSR,WDRF))
	{
		*ResVar = 1;
		sendAns();
		return;
	}
		initRam();
}

int main(void){
	
  initialisation();
    while (1) 
    {
		
    }
}