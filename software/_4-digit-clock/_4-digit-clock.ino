// letter n for displaying "on"
#define N 16
// special parameters to handle decimal point
#define DP_ON 17
// this is a general parameter to turn any segment off
#define BLANK 18

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

// settings state
// these values make the code easier
#define DISPLAY 9
#define SET_HR 0xA
#define SET_MIN 0xB
#define SET_CHIME 0xC
#define SET_ALARM 0xD
#define SET_ALARM_HR 0xE
#define SET_ALARM_MIN 0xF

void ledSetup(void);
void ledAm(void);
void ledPm(void);
void buttonsSetup(void);
void timer1Setup(void);
void nextSettingState(void);
void increaseHour(void);
void displaySetup(void);
// time(1234);
void displayDigits(int dig1, int dig2, int dp, int dig3, int dig4);
void setDigit(int digit, int number);
void setSegment(int number);


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
int settings_mode = DISPLAY;

// hourly chime on or off
int chime_enabled = 1;

// alarm on or off
int alarm_enabled = 1;
// alarm time
int alarm_hr = 12;
int alarm_min = 0;


void setup() {
  // put your setup code here, to run once:
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
}

void loop() {
  // put your main code here, to run repeatedly:
// display in loop so they keep updating
while(1)
{
  // default display mode
  if (settings_mode == DISPLAY)
  {
    // HH:MM
    if(hr_display)
    {
      displayDigits(hr/10, hr%10, dp, min_10, min_1);
    }
    // MM:SS
    else
    {
      displayDigits(min_10, min_1, dp, sec/10, sec%10);
    }
  }
  else if (settings_mode == SET_HR)
  {
    displayDigits(SET_HR, BLANK, BLANK, hr/10, hr%10);
  }
  else if (settings_mode == SET_MIN)
  {
    displayDigits(SET_MIN, BLANK, BLANK, min_10, min_1);
  }
  else if (settings_mode == SET_CHIME)
  {
    // on
    if (chime_enabled)
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
    if (alarm_enabled)
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
    displayDigits(SET_ALARM_HR, BLANK, BLANK, alarm_hr/10, alarm_hr%10);
  }
  else if (settings_mode == SET_ALARM_MIN)
  {
    displayDigits(SET_ALARM_MIN, BLANK, BLANK, alarm_min/10, alarm_min%10);
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

// increase hour by 1 and handle AM/PM
void increaseHour(void)
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
          increaseHour();
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
    increaseHour();
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
    // don't display leading 0
    if (number != 0)
    {
        PORTB &= ~(1<<PORTB2); // 1
    }
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
      // A B D F G
      PORTB |= (1<<PORTB1 | 1<<PORTB0); // A F
      PORTC |= (1<<PORTC3); // D
      PORTD |= (1<<PORTD5 | 1<<PORTD1); // B G
      break;
    case 0xF:
      // A B F G
      PORTB |= (1<<PORTB1 | 1<<PORTB0); // A F
      PORTD |= (1<<PORTD5 | 1<<PORTD1); // B G
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
