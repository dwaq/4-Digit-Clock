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

// differentiate between AM and PM
// 0 = AM // 1 = PM
int time_of_day = 1;

// default to display mode
Settings_TypeDef settings_mode = DISPLAY;

// hourly chime on or off
int chime_enabled = 1;

// alarm on or off
int alarm_enabled = 1;
// alarm time
int alarm_hr = 12;
int alarm_min = 0;


int main(void)
{
  // set display pins to outputs
  displaySetup();

  // set LED pin to output
  ledSetup();

  // set button pins to interrupt-enabled inputs
  buttonsSetup();

  // read port C to get default values
  port_c_history = PINC;

  // setup timer for 1 second tick
  timer1Setup();

  // enable interrupts (for both buttons and timer1)
  sei();

  // display in loop so they keep updating
  while(1)
  {
    // default display mode
    if (settings_mode == DISPLAY)
    {
      // HH:MM
      if(hr_display)
      {
        display(hr/10, hr%10, dp, min_10, min_1);
      }
      // MM:SS
      else
      {
        display(min_10, min_1, dp, sec/10, sec%10);
      }
    }
    else if (settings_mode == SET_HR)
    {
      display(SET_HR, BLANK, BLANK, hr/10, hr%10);
    }
    else if (settings_mode == SET_MIN)
    {
      display(SET_MIN, BLANK, BLANK, min_10, min_1);
    }
    else if (settings_mode == SET_CHIME)
    {
      // on
      if (chime_enabled)
      {
        display(SET_CHIME, BLANK, BLANK, 0, N);
      }
      // off
      else
      {
        display(SET_CHIME, 0, BLANK, 0xF, 0xF);
      }
    }
    else if (settings_mode == SET_ALARM)
    {
      // on
      if (alarm_enabled)
      {
        display(SET_ALARM, BLANK, BLANK, 0, N);
      }
      // off
      else
      {
        display(SET_ALARM, 0, BLANK, 0xF, 0xF);
      }
    }
    else if (settings_mode == SET_ALARM_HR)
    {
      display(SET_ALARM_HR, BLANK, BLANK, alarm_hr/10, alarm_hr%10);
    }
    else if (settings_mode == SET_ALARM_MIN)
    {
      display(SET_ALARM_MIN, BLANK, BLANK, alarm_min/10, alarm_min%10);
    }
  }
}

// correctly move through settings states
void nextSettingState(void)
{
  // if you've turned alarm off, dont't set it, go back to display
  if ((settings_mode == SET_ALARM) & (alarm_enabled == 0))
  {
    settings_mode = DISPLAY;
  }
  // last setting, go back to display
  else if (settings_mode == SET_ALARM_MIN)
  {
    settings_mode = DISPLAY;
  }
  // else just go to next state
  else{
    settings_mode++;
  }

  // stop counting time when you're trying to set it
  if (settings_mode == SET_HR)
  {
    // disable timer counter
    TIMSK1 &= ~(1 << OCIE1A);

    // also reset seconds for good measure
    sec = 0;
  }
  // start counting after you've set the time
  else if (settings_mode == SET_CHIME)
  {
    // start timer counter
    TIMSK1 |= (1 << OCIE1A);
  }
}

// AM/PM LED is on D4 PD4
void ledSetup(void)
{
  DDRD |= (1<<DDD4);
}

// low is AM
void ledAm(void)
{
  PORTD |= (1<<PORTD4);
}

// high is PM
void ledPm(void)
{
  PORTD &= ~(1<<PORTD4);
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
        // go to next settings state
        nextSettingState();
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
        else if (settings_mode == SET_HR)
        {
          if (++hr==13)
          {
            hr=1;

            // flip between AM and PM
            time_of_day ^= 1;

            // change LED to match
            if (time_of_day)
            {
              ledPm();
            }
            else{
              ledAm();
            }
          }
        }
        else if (settings_mode == SET_MIN)
        {
          if (++min_1==10)
          {
            min_1=0;
            if (++min_10==6)
            {
              min_10=0;
            }
          }
        }
        else if (settings_mode == SET_CHIME)
        {
          // switch between chime enabled and disabled
          chime_enabled ^= 1;
        }
        else if (settings_mode == SET_ALARM)
        {
          // switch between alarm enabled and disabled
          alarm_enabled ^= 1;
        }
        else if (settings_mode == SET_ALARM_HR)
        {
          if (++alarm_hr==13)
          {
            alarm_hr=1;
          }
        }
        else if (settings_mode == SET_ALARM_MIN)
        {
          if (++alarm_min==60)
          {
            alarm_min=1;
          }
        }
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

  // set interrupt on compare match
  TIMSK1 |= (1 << OCIE1A);

  // set prescaler to 1024
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

      // flip between AM and PM
      time_of_day ^= 1;

      // change LED to match
      if (time_of_day)
      {
        ledPm();
      }
      else{
        ledAm();
      }
		}
	}

	// flip decimal point every second
	dp ^= 1;
}
