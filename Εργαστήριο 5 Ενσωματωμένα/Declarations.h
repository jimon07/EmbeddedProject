
#ifndef INCFILE1_H_
#define INCFILE1_H_

// Upper and Lower Byte Declarations
#define HIGH(x)  ((x) >> 8)
#define LOW(x)  ((x) & 0xFF)
// Register names
#define	temp  r16
#define	temp1  r17
#define	AnsReg  r18
#define	Seg_data_out  r19
#define	LoopValue 10000	// Loops to count
#define	F_CLK 10000000 // 10 MHz clock Freq
#define	baud 9600		// Baud Rate
#define	UBRRValue F_CLK/16/baud- 1 // UBRR calculation According to Data sheet

// 7segment values for Display of Digits
#define	seg_clear 0b11111111
#define	seg_zero 0b11000000
#define	seg_one 0b11111001
#define	seg_two 0b10100100
#define	seg_three 0b10110000
#define	seg_four 0b10011001
#define	seg_five 0b10010010
#define	seg_six 0b10000010
#define	seg_seven 0b11111000
#define	seg_eight 0b10000000
#define	seg_nine 0b10011000
// ASCII Values
#define	ASCII_LF 0b00001010
#define	ASCII_CR 0b00001101
#define	ASCII_A 0b01000001
#define	ASCII_C 0b01000011
#define	ASCII_K 0b01001011
#define	ASCII_N 0b01001110
#define	ASCII_O 0b01001111
#define	ASCII_T 0b01010100



#endif /* INCFILE1_H_ */