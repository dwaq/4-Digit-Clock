#include "display.h"

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
void display(int dig1, int dig2, int dp, int dig3, int dig4)
{
  digit(1, dig1);
  digit(2, dig2);
  digit(3, dig3);
  digit(4, dig4);
  if (dp)
  {
  	// display decimal point (special parameter)
  	digit(2, DP_ON);
  }
  else
  {
  	// dont't display decimal point (special parameter)
  	digit(2, DP_OFF);
  }

}

// display a number on the selected digit 1, 2, 3, or 4
// TODO: add range checks
void digit(int digit, int number)
{
  // turn all digits off
  PORTB |= (1<<PORTB2); // 1
  PORTD |= (1<<PORTD7 | 1<<PORTD6 | 1<<PORTD2); // 2 3 4

  // set up segments while digits off,
  // so that the full digit comes up simultanously
  // (although who is going to be able to detect a couple clk cycles?)
  segment(number);

  // then turn on specfic digit
  switch(digit)
  {
    case 1:
      PORTB &= ~(1<<PORTB2); // 1
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

// display a number using the 7 segments
// number must be between 0-9
// TODO: add range checks
void segment(int number)
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
    case 10:
	  // DP
	  PORTC |= (1<<PORTC4);
	  break;
    //default:
	  // case 11 falls through here
	  // just turns off the decimal point by initial "reset all segments" code
      // bad - should't be here
  }
}
