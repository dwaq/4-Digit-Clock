#include <avr/io.h>
#include <avr/interrupt.h>

#include "main.h"
#include "display.h"

// start high but will read to get real defaults
volatile uint8_t port_c_history = 0xFF;

// displaying time
int sec = 0;
int min_1 = 0;
int min_10 = 0;
int hr = 12;

// decimal point
int dp = 0;

// switch between HH:MM and MM:SS
int hr_display = 1;

// default to display mode
Settings_TypeDef settings_mode = DISPLAY;

int main(void)
{
  // set display pins to outputs
  displaySetup();

  // set button pins to interrupt-enabled inputs
  buttonsSetup();

  // read port C to get default values
  port_c_history = PINC;

  // setup timer for 1 second tick
  timer1Setup();

  // enable interrupts (for both buttons and timer1)
  sei();

  while(1)
  {
    // display all segments in loop so they keep updating
    if(hr_display)
    {
      display(hr/10, hr%10, dp, min_10, min_1);
    }
    else
    {
      display(min_10, min_1, dp, sec/10, sec%10);
    }
  }
}

// S1 Hr  left  A0  PC0 PCINT8
// S2 Min right A1  PC1 PCINT9
// https://sites.google.com/site/qeewiki/books/avr-guide/external-interrupts-on-the-atmega328
void buttonsSetup(void)
{
  // clear the pins (set them to input)
  DDRC &= ~(1 << DDC1 | 1 << DDC0);

  // turn on the internal pull-up resistors
  PORTC |= (1 << PORTC1 | 1 << PORTC0);

  // any change on any enabled PCINT[14:8] pin will cause an interrupt
  PCICR |= (1 << PCIE1);

  // pin change interrupt is enabled on the corresponding I/O pin
  PCMSK1 |= (1 << PCINT9 | 1 << PCINT8);
}

ISR (PCINT0_vect)
{
    uint8_t changedbits;

    changedbits = PINC ^ port_c_history;
    port_c_history = PINC;

    // S1 changed
    if(changedbits & (1 << PINC0))
    {
      // falling edge (pressed)
      if((PINC & (1 << PINC0)) == 0)
      {
        // go to settings
      }
      // rising edge (released)
      else
      {
        // do we care about this?
      }
    }

    // S2 changed
    if(changedbits & (1 << PINC1))
    {
      // falling edge (pressed)
      if((PINC & (1 << PINC1)) == 0)
      {
        // need to be in display mode to change
        if (settings_mode == DISPLAY)
        {
          // switch between HH:MM and MM:SS
          hr_display ^= 1;
        }
        // else in some settings mode...
      }
      // rising edge (released)
      else
      {
        // do we care about this?
      }
    }
}

// this code sets up timer1 for a 1s  @ 16Mhz Clock (mode 4)
// https://sites.google.com/site/qeewiki/books/avr-guide/timers-on-the-atmega328
void timer1Setup(void)
{
  // Mode 4, CTC on OCR1A
  OCR1A = 0x3D08;
  TCCR1B |= (1 << WGM12);

  //Set interrupt on compare match
  TIMSK1 |= (1 << OCIE1A);

  // set prescaler to 1024 and start the timer
  TCCR1B |= (1 << CS12) | (1 << CS10);
}

// action to be done every 1 sec
ISR (TIMER1_COMPA_vect)
{
	//keep track of time
	// using 2 instead of 60 to make it faster
	if (++sec==2)
	{
		sec=0;
		// minutes
		if (++min_1==10)
		{
			min_1=0;
			if (++min_10==6)
			{
				min_10=0;
				// TODO: hours code should actually be here...
			}
		}
		// hours
		if (++hr==13)
		{
			hr=1;
		}
	}

	// flip decimal point every second
	dp ^= 1;
}
