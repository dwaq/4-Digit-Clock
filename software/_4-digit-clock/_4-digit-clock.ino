#include <TimerOne.h>

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

// These values make the code easier, because you can
// use the same values for display during the setting mode
// and they're sequential so you can increment through them
// settings states
const int CLOCK = 9;
const int SET_HR = 0xA;
const int SET_MIN = 0xB;
const int SET_CHIME = 0xC;
const int SET_ALARM = 0xD;
const int SET_ALARM_HR = 0xE;
const int SET_ALARM_MIN = 0xF;
// letter n for displaying "on"
const int N = 0x10;
// special parameters to handle decimal point
const int DP_ON = 0x11;
// this is a general parameter to turn any segment off
const int BLANK = 0x12;

void ledSetup(void);
void ledAm(void);
void ledPm(void);

void buttonsSetup(void);
void buttonsS1(void);
void buttonsS2(void);

void nextSettingState(void);

void increaseSecond(void);
void increaseHour(void);

void displaySetup(void);
void displayDigits(int dig1, int dig2, int dp, int dig3, int dig4);
void setDigit(int digit, int number);
void setSegment(int number);

// time
struct TimeType {
  int sec;
  int min;
  int hr;
  // differentiate between AM and PM
  // 0 = AM // 1 = PM
  int time_of_day;
};
TimeType time;

// decimal point
int dp = 0;

// switch between HH:MM and MM:SS
int hr_display = 1;

// default to clock display mode
int settings_mode = CLOCK;

// hourly chime on or off
int hr_chime_enabled = 1;

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

// capture only 1 button press (kinda like debouncing)
int buttonBlockA0 = 0;
int buttonBlockA1 = 0;

void setup() {
  // set up default time values
  time.sec = 0;
  time.min = 0;
  time.hr = 12;
  time.time_of_day = 1;

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
    if(hr_display)
    {
      displayDigits(time.hr/10, time.hr%10, dp, time.min/10, time.min%10);
    }
    // MM:SS
    else
    {
      displayDigits(time.min/10, time.min%10, dp, time.sec/10, time.sec%10);
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
    hr_display ^= 1;
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
	//keep track of time
	// using 2 instead of 60 to make it faster
	if (++time.sec==60)
	{
		time.sec=0;
		// minutes
		if (++time.min==60)
		{
			time.min=0;
			increaseHour();
		}
	}

	// flip decimal point every second
	dp ^= 1;
}


// set display pins to outputs
void displaySetup()
{
  DDRB |= (1<<DDB1 | 1<<DDB0 | 1<<DDB2); // A F 1
  DDRC |= (1<<DDC3 | 1<<DDC2 | 1<<DDC4); // D E DP
  DDRD |= (1<<DDD5 | 1<<DDD0 | 1<<DDD1 | 1<<DDD7 | 1<<DDD6 | 1<<DDD2); // B C G 2 3 4
}

// cycle through and display each digit
// dp is for decimal point: 0 for off; 1 for on
// TODO: make dp a bool?
// TODO: put dp last so it doesn't affect dig2's brightness?
void displayDigits(int dig1, int dig2, int dp, int dig3, int dig4)
{
  setDigit(1, dig1);
  setDigit(2, dig2);
  setDigit(3, dig3);
  setDigit(4, dig4);
  if (dp)
  {
    // display decimal point (special parameter)
    setDigit(2, DP_ON);
  }
  else
  {
    // dont't display decimal point (special parameter)
    setDigit(2, BLANK);
  }

}

// display a number on the selected digit 1, 2, 3, or 4
// TODO: add range checks
void setDigit(int digit, int number)
{
  // turn all digits off
  PORTB |= (1<<PORTB2); // 1
  PORTD |= (1<<PORTD7 | 1<<PORTD6 | 1<<PORTD2); // 2 3 4

  // set up segments while digits off,
  // so that the full digit comes up simultanously
  // (although who is going to be able to detect a couple clk cycles?)
  setSegment(number);

  // then turn on specfic digit
  switch(digit)
  {
    case 1:
    // TODO: ONLY when display hours,
    // don't display leading 0
    //if (number != 0)
    //{
        PORTB &= ~(1<<PORTB2); // 1
    //}
      break;
    // this case also handles decimal point
    case 2:
      PORTD &= ~(1<<PORTD7); // 2
      break;
    case 3:
      PORTD &= ~(1<<PORTD6); // 3
      break;
    case 4:
      PORTD &= ~(1<<PORTD2); // 4
      break;
    //default:
      // bad - should't be here
  }
}

// display a number or symbol using the 7 segments
// number can be between 0-9 or settings screen uses A-F
// TODO: add range checks
void setSegment(int number)
{
  // first, reset all segments
  PORTB &= ~(1<<PORTB1 | 1<<PORTB0); // A F
  PORTC &= ~(1<<PORTC4 | 1<<PORTC3 | 1<<PORTC2); // DP D E
  PORTD &= ~(1<<PORTD5 | 1<<PORTD0 | 1<<PORTD1); // B C G

  // then set the coorespoding segments
  switch(number)
  {
    case 0:
      // A B C D E F
      PORTB |= (1<<PORTB1 | 1<<PORTB0); // A F
      PORTC |= (1<<PORTC3 | 1<<PORTC2); // D E
      PORTD |= (1<<PORTD5 | 1<<PORTD0); // B C
      break;
    case 1:
      // B C
      PORTD |= (1<<PORTD5 | 1<<PORTD0); // B C
      break;
    case 2:
      // A B D E G
      PORTB |= (1<<PORTB1); // A
      PORTC |= (1<<PORTC3 | 1<<PORTC2); // D E
      PORTD |= (1<<PORTD5 | 1<<PORTD1); // B G
      break;
    case 3:
      // A B C D G
      PORTB |= (1<<PORTB1); // A
      PORTC |= (1<<PORTC3); // D E
      PORTD |= (1<<PORTD5 | 1<<PORTD0 | 1<<PORTD1); // B C G
      break;
    case 4:
      // B C F G
      PORTB |= (1<<PORTB0); // F
      PORTD |= (1<<PORTD5 | 1<<PORTD0 | 1<<PORTD1); // B C G
      break;
    case 5:
      // A C D F G
      PORTB |= (1<<PORTB1 | 1<<PORTB0); // A F
      PORTC |= (1<<PORTC3); // D
      PORTD |= (1<<PORTD0 | 1<<PORTD1); // C G
      break;
    case 6:
      // A C D E F G
      PORTB |= (1<<PORTB1 | 1<<PORTB0); // A F
      PORTC |= (1<<PORTC3 | 1<<PORTC2); // D E
      PORTD |= (1<<PORTD0 | 1<<PORTD1); // C G
      break;
    case 7:
      // A B C
      PORTB |= (1<<PORTB1); // A
      PORTD |= (1<<PORTD5 | 1<<PORTD0); // B C
      break;
    case 8:
      // A B C D E F G
      PORTB |= (1<<PORTB1 | 1<<PORTB0); // A F
      PORTC |= (1<<PORTC3 | 1<<PORTC2); // D E
      PORTD |= (1<<PORTD5 | 1<<PORTD0 | 1<<PORTD1); // B C G
      break;
    case 9:
      // A B C D F G
      PORTB |= (1<<PORTB1 | 1<<PORTB0); // A F
      PORTC |= (1<<PORTC3); // D
      PORTD |= (1<<PORTD5 | 1<<PORTD0 | 1<<PORTD1); // B C G
      break;
    // TODO: maybe use typedefs here instead?
    case 0xA:
      // A B C E F G
      PORTB |= (1<<PORTB1 | 1<<PORTB0); // A F
      PORTC |= (1<<PORTC2); // E
      PORTD |= (1<<PORTD5 | 1<<PORTD0 | 1<<PORTD1); // B C G
      break;
    case 0xB:
      // A B C D E F G
      PORTB |= (1<<PORTB1 | 1<<PORTB0); // A F
      PORTC |= (1<<PORTC3 | 1<<PORTC2); // D E
      PORTD |= (1<<PORTD5 | 1<<PORTD0 | 1<<PORTD1); // B C G
      break;
    case 0xC:
      // A D E F
      PORTB |= (1<<PORTB1 | 1<<PORTB0); // A F
      PORTC |= (1<<PORTC3 | 1<<PORTC2); // D E
      break;
    case 0xD:
      // A B C D E F
      PORTB |= (1<<PORTB1 | 1<<PORTB0); // A F
      PORTC |= (1<<PORTC3 | 1<<PORTC2); // D E
      PORTD |= (1<<PORTD5 | 1<<PORTD0); // B C
      break;
    case 0xE:
      // A D E F G
      PORTB |= (1<<PORTB1 | 1<<PORTB0); // A F
      PORTC |= (1<<PORTC3 | 1<<PORTC2); // D E
      PORTD |= (1<<PORTD1); // G
      break;
    case 0xF:
      // A E F G
      PORTB |= (1<<PORTB1 | 1<<PORTB0); // A F
      PORTC |= (1<<PORTC2); // E
      PORTD |= (1<<PORTD1); // G
      break;
    case N:
      // A B C E F
      PORTB |= (1<<PORTB1 | 1<<PORTB0); // A F
      PORTC |= (1<<PORTC2); // E
      PORTD |= (1<<PORTD5 | 1<<PORTD0); // B C
      break;
    case DP_ON:
      // DP
      PORTC |= (1<<PORTC4);
      break;
    //default:
      // case BLANK falls through here
      // just turns off the segment by initial "reset all segments" code
  }
}
