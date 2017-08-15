#ifndef DISPLAY_H_
#define DISPLAY_H_

#include <avr/io.h>

// special parameters to handle decimal point
#define DP_ON 10
#define DP_OFF 11

// pinouts for common cathode 7 segment LED display:

// segment is active high
// segment | display | header | mcu
// A  11 D9  PB1
// B  7  D5  PD5
// C  4  D0  PD0
// D  2  A3  PC3
// E  1  A2  PC2
// F  10 D8  PB0
// G  5  D1  PD1
// DP 3  A4  PC4

// digit is active low
// digit | display | header | mcu
// 1  12 D10 PB2
// 2  9  D7  PD7
// 3  8  D6  PD6
// 4  6  D2  PD2

void displaySetup(void);
// time(1234);
void display(int dig1, int dig2, int dp, int dig3, int dig4);
void digit(int digit, int number);
void segment(int number);

#endif // DISPLAY_H_
