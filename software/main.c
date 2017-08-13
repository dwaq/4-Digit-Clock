#include <avr/io.h>
#include <avr/interrupt.h>

#include "display.h"

// http://www.microchip.com//wwwAppNotes/AppNotes.aspx?appnote=en591232

int sec = 0;
int min_1 = 0;
int min_10 = 0;
int hr_1 = 2;
int hr_10 = 1;

// decimal point
int dp = 0;

int main(void)
{
  // set pins to outputs
  displaySetup();

  // this code sets up timer1 for a 1s  @ 16Mhz Clock (mode 4)
  // https://sites.google.com/site/qeewiki/books/avr-guide/timers-on-the-atmega328
  
  // Mode 4, CTC on OCR1A
  OCR1A = 0x3D08;
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
 	display(hr_10, hr_1, dp, min_10, min_1);
  }
}

// action to be done every 1 sec
ISR (TIMER1_COMPA_vect)
{
	//keep track of time
	// using 2 instead of 60 to make it faster
	/*
	if (++sec==2)        
	{
		sec=0;
		if (++min_1==10)
		{
			min_1=0;
			if (++min_10==6)
			{
				min_10=0;
			}
		}
	}
	*/

	// flip decimal point every second
	dp ^= 1;
}

