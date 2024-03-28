
#ifndef INCFILE1_H_
#define INCFILE1_H_

// Upper and Lower Byte Declarations
#define HIGH(x)  ((x) >> 8)
#define LOW(x)  ((x) & 0xFF)
// Register names
#define	LoopValue 10000	// Loops to count
#define	F_CLK 10000000 // 10 MHz clock Freq
#define	baud 9600		// Baud Rate
#define	UBRRValue F_CLK/16/baud- 1 // UBRR calculation According to Data sheet

// ASCII Values
#define	ASCII_LF 0b00001010
#define	ASCII_CR 0b00001101
#define	ASCII_A 0b01000001
#define	ASCII_C 0b01000011
#define	ASCII_K 0b01001011
#define	ASCII_N 0b01001110
#define	ASCII_O 0b01001111
#define ASCII_R 0b01010010
#define	ASCII_T 0b01010100

#endif /* INCFILE1_H_ */