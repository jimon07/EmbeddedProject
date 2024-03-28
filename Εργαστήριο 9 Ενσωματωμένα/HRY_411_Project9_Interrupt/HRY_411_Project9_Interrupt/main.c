/*
 * HRY_411_Project9_Interrupt.c
 *
 * Created: 17/12/2020
 * Dimitris Ntounetas
 * AM:2016030141
 */ 

/*
	Switch debouncing with External Interrupts.
	The logic here is that first the switch that stays stable
	changes and we know that a change is made if both have changed
	then if one of the 2 interrupts come more than 2 times 
	which means that it bounced one time then we know that this is
	the bouncing side. We output the A side if A is stable or we 
	output the compliment of (A') side if A' is stable.
*/
	
#include <avr/io.h>
#include <avr/interrupt.h>
#include "Declarations.h"


//	Switch A is PORTA bit 0 and Switch A' is PORTA bit 1
//	Pointer Creations

// 2 bits for switch checking
volatile uint8_t *count = (uint8_t *)0x0080;
volatile uint8_t *count1 = (uint8_t *)0x0081;
volatile uint8_t *lastBounced = (uint8_t *)0x0082;

void Ext_Int_0(){
	
	if (*count > *count1 && *count >=2 && *count1 == 0 && *lastBounced == 1)
	{
		// Means that A in PD2 is bouncing PD3(A') => Stable
		PORTA = 0b00000100&(~(PIND&(1<<PD3))>> 1);
		*lastBounced = 0;	
	}
	else if(*count > 0 && *count1 > 0)
	{
		*count = 0;
		*count1 = 0;
	}
	else
	{
		*count += 1;	
	}
}
void Ext_Int_1(){
	
	if (*count1 > *count && *count1 >=2 && *count == 0 && *lastBounced == 0)
	{
		// Means that A' in PD3 is bouncing PD2(A) => Stable
		PORTA = 0b00000100 &((PIND&(1<<PD2)));
		*lastBounced = 1;
	}
	else if(*count > 0 && *count1 > 0)
	{
		*count = 0;
		*count1 = 0;
	}
	else{	
		*count1 += 1;
	}
}

// Interrupt Service Routines
ISR(INT0_vect)
{
	Ext_Int_0();
}
ISR(INT1_vect)
{
	Ext_Int_1();	
}
void initPorts(){
	
// 	Ports Initialization
	DDRA = 0b00000100;	//Port A Initialization: bit 2 as output and the others as inputs
	DDRD = 0b00000000;	//PortD as Input
	PORTA = 0b00000100;
	
// 	Any logical change on PD2 and PD3 generates a request
	MCUCR = (1<<ISC10)|(1<<ISC00);
//	Enable interrupts from PD2 and PD3
	GICR = (1<<INT0)|(1<<INT1);
//	Initialisation of counters
	*count = 0;
	*count1 = 0;
//	Start position;
	*lastBounced = 0;
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