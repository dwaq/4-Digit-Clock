#include "display.h"
#include "main.h"
#include <wiring_private.h>

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

// set display pins to outputs
void displaySetup()
{
  DDRB |= (1<<DDB1 | 1<<DDB0 | 1<<DDB2); // A F 1
  DDRC |= (1<<DDC3 | 1<<DDC2 | 1<<DDC4); // D E DP
  DDRD |= (1<<DDD5 | 1<<DDD0 | 1<<DDD1 | 1<<DDD7 | 1<<DDD6 | 1<<DDD2); // B C G 2 3 4
}

// cycle through and display each digit
// dp is for decimal point: 0 for off; 1 for on
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
