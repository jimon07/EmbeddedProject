/*
 * HRY_411_Project9.c
 *
 * Created: 17/12/2020
 * Dimitris Ntounetas
 * AM:2016030141
 */ 

/*
	Switch debouncing with polling and a Timer
	The logic here is that by polling the switch,
	if the switch has the same values in A and A'
	for 10 consecutive times, then we are pretty sure
	that its value is stable and we output it. 
*/
	
#include <avr/io.h>
#include <avr/interrupt.h>
#include "Declarations.h"


//	Switch A is PORTA bit 0 and Switch A' is PORTA bit 1
//	Pointer Creations

// 2 bits for switch checking
volatile uint8_t *switch1 = (uint8_t *)0x0090;
volatile uint8_t *prev = (uint8_t *)0x0091;
volatile uint8_t *count = (uint8_t *)0x0092;

void CTC_Timer_Interrupt(){
	
// 	bit 0 = A and bit 1 = A'
	*switch1 = (PINA&(1<<PA0))|(PINA&(1<<PA1));
	
//	if both switch values are the same reset counter
	if ((*switch1 == 3) || (*switch1 == 0))
	{
		*count = 0;
	}
//	if both switch values are the same start counting
	if (((*switch1 == 1)||(*switch1 == 2))&&(*prev == *switch1)){
		*count +=1;
	}
//	if for 10 consecutive times we have the same values then the switch is stable
	if (*count == 10)
	{	
		// Output the correct output in PORTA bit 2 including previous values of PORTA.
		PORTA= ((PINA&(1<<PA0)) << PA2);
		
		*count = 0;
	}
//	Save the switch values to evaluate next time
	*prev = *switch1;
	return;
}
// Interrupt Service Routines
ISR(TIMER1_COMPA_vect)
{
	CTC_Timer_Interrupt();
}
void initPorts(){
	
// 	Ports Initialization
	DDRA = 0b00000100;	//Port A Initialization: bit 2 as output and the others as inputs

//	Enable Compare Interrupt
	TIMSK = (1<<OCIE1A);
// 	Compare Register gets value of 625 for 0.5 ms
	OCR1AH = HIGH(LoopValue);
	OCR1AL = LOW(LoopValue);
//	Prescale 1/8 and CTC mode
	TCCR1B = (1<<CS11)|(1<<WGM12);
	*switch1 = 0b00000000; 
	*switch1 = 0b00000000; 
}
void initialisation(void){
	
//	Chip Initializations
	initPorts();
	sei();
}
int main(void)
{
   initialisation();
    while (1) 
    {
    }
}