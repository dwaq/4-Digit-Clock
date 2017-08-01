#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>

#include "display.h"

int main(void)
{
  // set pins to outputs
  displaySetup();

  // can't use this blocking method
  //_delay_ms(1000);  //Delay for 1000ms => 1 sec

  while(1)
  {
    // display all segments in loop so they keep updating
    display(8,8,8,8);
  }
}
