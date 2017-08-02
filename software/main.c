#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>

#include "display.h"

int count = 0;
int d = 0;

int main(void)
{
  // set pins to outputs
  displaySetup();

  // can't use this blocking method
  //_delay_ms(1000);  //Delay for 1000ms => 1 sec

  while(1)
  {
  	// display all segments in loop so they keep updating
  	while (count < 25000)
  	{
      // half the time the dp is on
  		if (count < 12500)
  		{
  			// decimal point
  			display(d,d,1,d,d);
  		}
      // half the time it's off
  		else
  		{
  			// no decimal point
  			display(d,d,0,d,d);
  		}
  		count++;
  	}
  	count = 0;

    // go to the next digit
  	d++;
    // can't display past 9
  	if (d == 10)
  	{
  		d=0;
  	}
  }
}
