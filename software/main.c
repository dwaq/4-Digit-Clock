#include <avr/io.h>
#include <avr/interrupt.h>

#include "display.h"

int count = 0;
int d = 0;
int dp = 0;

int main(void)
{
  // set pins to outputs
  displaySetup();

  // this code sets up timer1 for a 1s  @ 16Mhz Clock (mode 4)
  OCR1A = 0x3D08;

  // Mode 4, CTC on OCR1A
  TCCR1B |= (1 << WGM12);

  //Set interrupt on compare match
  TIMSK1 |= (1 << OCIE1A);  

  // set prescaler to 1024 and start the timer
  TCCR1B |= (1 << CS12) | (1 << CS10);

  // enable interrupts
  sei();

  while(1)
  {
  	// display all segments in loop so they keep updating
 	display(d,d,dp,d,d);
  }
}

// action to be done every 1 sec
ISR (TIMER1_COMPA_vect)
{
	// go to the next digit
  	d++;
    // can't display past 9
  	if (d == 10)
  	{
  		d=0;
  	}

	// flip decimal point
	dp ^= 1;
}
