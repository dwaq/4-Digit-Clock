#include "main.h"
#include "display.h"
#include <TimerOne.h>

// time
struct TimeType {
  int sec;
  int min;
  int hr;
  // differentiate between AM and PM
  // 0 = AM // 1 = PM
  int time_of_day;
  // decimal point
  int dp;
  // switch between HH:MM and MM:SS
  int display;
};
TimeType time;

// alarm
struct AlarmType {
  // alarm on or off
  int enabled;
  // alarm time
  int hr;
  int min;
  // AM/PM for alarm
  // 0 = AM // 1 = PM
  int time_of_day;
};
AlarmType alarm;

// default to clock display mode
int settings_mode = CLOCK;

// hourly chime on or off
int hr_chime_enabled = 1;

// capture only 1 button press (kinda like debouncing)
int buttonBlockA0 = 0;
int buttonBlockA1 = 0;

void setup() {
  // set up default time values
  time.sec = 0;
  time.min = 0;
  time.hr = 12;
  time.time_of_day = 1;
  time.dp = 0;
  time.display = 1;

  // set up default alarm values
  alarm.hr = 12;
  alarm.min = 0;
  alarm.time_of_day = 1;
  alarm.enabled = 1;

  // set display pins to outputs
  displaySetup();

  // set LED pin to output
  ledSetup();

  // set button pins to interrupt-enabled inputs
  buttonsSetup();

  // setup timer for 1 second tick
  Timer1.initialize(1000000);
  Timer1.attachInterrupt(increaseSecond);
}

void loop() {
  // displayDigits in loop so they keep updating

  // default clock display mode
  if (settings_mode == CLOCK)
  {
    // HH:MM
    if(time.display)
    {
      displayDigits(time.hr/10, time.hr%10, time.dp, time.min/10, time.min%10);
    }
    // MM:SS
    else
    {
      displayDigits(time.min/10, time.min%10, time.dp, time.sec/10, time.sec%10);
    }
  }
  else if (settings_mode == SET_HR)
  {
    displayDigits(SET_HR, BLANK, BLANK, time.hr/10, time.hr%10);
  }
  else if (settings_mode == SET_MIN)
  {
    displayDigits(SET_MIN, BLANK, BLANK, time.min/10, time.min%10);
  }
  else if (settings_mode == SET_CHIME)
  {
    // on
    if (hr_chime_enabled)
    {
      displayDigits(SET_CHIME, BLANK, BLANK, 0, N);
    }
    // off
    else
    {
      displayDigits(SET_CHIME, 0, BLANK, 0xF, 0xF);
    }
  }
  else if (settings_mode == SET_ALARM)
  {
    // on
    if (alarm.enabled)
    {
      displayDigits(SET_ALARM, BLANK, BLANK, 0, N);
    }
    // off
    else
    {
      displayDigits(SET_ALARM, 0, BLANK, 0xF, 0xF);
    }
  }
  else if (settings_mode == SET_ALARM_HR)
  {
    displayDigits(SET_ALARM_HR, BLANK, BLANK, alarm.hr/10, alarm.hr%10);
  }
  else if (settings_mode == SET_ALARM_MIN)
  {
    displayDigits(SET_ALARM_MIN, BLANK, BLANK, alarm.min/10, alarm.min%10);
  }
}

// correctly move through settings states
void nextSettingState(void)
{
  // if you've turned alarm off, dont't set it, go back to clock display
  if ((settings_mode == SET_ALARM) & (alarm.enabled == 0))
  {
    settings_mode = CLOCK;
  }
  // last setting, go back to clock display
  else if (settings_mode == SET_ALARM_MIN)
  {
    settings_mode = CLOCK;
  }
  // else just go to next state
  else{
    settings_mode++;
  }

  // stop counting time when you're trying to set it
  if (settings_mode == SET_HR)
  {
    // disable timer counter
    Timer1.stop();

    // also reset seconds for good measure
    time.sec = 0;
  }
  // start counting after you've set the time
  else if (settings_mode == SET_CHIME)
  {
    // restart timer counter
    Timer1.restart();
  }
}

// increase hour by 1 and handle AM/PM
void increaseHour(void)
{
  // play chime on the hour if enabled
  if (hr_chime_enabled)
  {
    // send 133 frequency tone for 70 ms to pin 3
    tone(3, 133, 70);
  }

  if (++time.hr==13)
  {
    time.hr=1;

    // flip between AM and PM
    time.time_of_day ^= 1;

    // change LED to match
    if (time.time_of_day)
    {
      ledPm();
    }
    else{
      ledAm();
    }
  }
}

// Hack to get pin change interrupts on analog pins
// http://www.geertlangereis.nl/Electronics/Pin_Change_Interrupts/PinChange_en.html
void buttonsSetup()
{
  // set button pins as inputs with internal pull-ups enabled
  pinMode(A0, INPUT);
  digitalWrite(A0, HIGH);
  pinMode(A1, INPUT);
  digitalWrite(A1, HIGH);

  // switch interrupts off while messing with their settings
  cli();
  // Enable PCINT1 interrupt
  PCICR =0x02;
  PCMSK1 = 0b00000111;
  // turn interrupts back on
  sei();
}

// Interrupt service routine for every analog pin change
// A0, A1, A2, A3, A4, A5
ISR(PCINT1_vect) {
  // press A0
  if (digitalRead(A0)==0 && buttonBlockA0==0)
  {
    // set to 10
    buttonBlockA0 = 10;
    // send 133 frequency tone for 70 ms to pin 3
    tone(3, 133, 70);
    buttonS1();
  }
  // release A0
  if (digitalRead(A0)==1)
  {
    // count down after releasing
    if (buttonBlockA0 > 0)
    {
      buttonBlockA0--;
    }
  }

  // press A1
  if (digitalRead(A1)==0 && buttonBlockA1==0)
  {
    // set to 10
    buttonBlockA1 = 10;
    // send 133 frequency tone for 70 ms to pin 3
    tone(3, 133, 70);
    buttonS2();
  }
  // release A1
  if (digitalRead(A1)==1)
  {
    // count down after releasing
    if (buttonBlockA1 > 0)
    {
      buttonBlockA1--;
    }
  }
}

void buttonS1()
{
  // go to next settings state
  nextSettingState();
}

void buttonS2()
{
  // need to be in clock display mode to change these settings
  if (settings_mode == CLOCK)
  {
    // switch between HH:MM and MM:SS
    time.display ^= 1;
  }
  // else in some settings mode...
  else if (settings_mode == SET_HR)
  {
    increaseHour();
  }
  else if (settings_mode == SET_MIN)
  {
    if (++time.min==60)
    {
      time.min=0;
    }
  }
  else if (settings_mode == SET_CHIME)
  {
    // switch between hourly chime enabled and disabled
    hr_chime_enabled ^= 1;
  }
  else if (settings_mode == SET_ALARM)
  {
    // switch between alarm enabled and disabled
    alarm.enabled ^= 1;
  }
  else if (settings_mode == SET_ALARM_HR)
  {
    if (++alarm.hr==13)
    {
      alarm.hr=1;
      // flip between AM and PM
      alarm.time_of_day ^= 1;
      // TODO: set LED coorespoding to time
      // but rememeber to unset it when moving to next state
    }
  }
  else if (settings_mode == SET_ALARM_MIN)
  {
    if (++alarm.min==60)
    {
      alarm.min=1;
    }
  }
}

// action to be done every 1 sec
void increaseSecond()
{
	// increase seconds
	if (++time.sec==60)
	{
		time.sec=0;
		// increase minute every 60 seconds
		if (++time.min==60)
		{
			time.min=0;
      // increase hour every 60 seconds
			increaseHour();
		}
	}

	// flip decimal point every second
	time.dp ^= 1;
}