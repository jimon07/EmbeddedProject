/*
 * HRY_411_Project10.c
 *
 * Created: 23/12/2020
 * Author : jim_1
 */ 

#include <avr/io.h>
#include <stdio.h>

volatile float *matrix1 = (float *)0x0070;
volatile float *matrix2 = (float *)0x0094;
volatile float *output = (float *)0x00B8;

void initialization(){
	for (uint8_t i = 0; i<9; i++)
	{	
		matrix1[i] = i;
	}
	for (uint8_t i = 0; i<9; i++)
	{
		matrix2[i] = i + 0.5879;
	}
	for(uint8_t i=0;i<9;i++)
	{
		output[i] = 0;
	}
}
void matrixmult(){
	for(uint8_t i=0; i<3; i++)
	{
		for(uint8_t j=0; j<3; j++)
		{
			
		output[3*i+j] = matrix1[3*i]*matrix2[j] + matrix1[3*i+1]*matrix2[j+3] + matrix1[3*i+2]*matrix2[j+6];
		
		}
	}
	
}
int main(void)
{
    initialization();
	matrixmult();
    while (1) 
    {
		
    }
}
