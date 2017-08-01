#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>

#include "display.h"

int main(void)
{

  int y = foo(3);  /* Use the function here */

  DDRB |= (1<<DDB5); //Set the 6th bit on PORTB (i.e. PB5) to 1 => output
  while(1)
  {
    PORTB |= (1<<PORTB5);    //Turn 6th bit on PORTB (i.e. PB5) to 1 => on
    _delay_ms(1000);        //Delay for 1000ms => 1 sec
    PORTB &= ~(1<<PORTB5);    //Turn 6th bit on PORTB (i.e. PB5) to 0 => off
    _delay_ms(1000);        //Delay for 1000ms => 1 sec
  }
}
