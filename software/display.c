#include "display.h"

// cycle through and display each digit
void display(int dig1, int dig2, int dig3, int dig4)
{
  digit(1, dig1);
  digit(2, dig2);
  digit(3, dig3);
  digit(4, dig4);
}

// display a number on the selected digit 1, 2, 3, or 4
// TODO: add range checks
void digit(int digit, int number)
{
  // turn all digits off
  PORTB &= ~(1<<PORTB2); // 1
  PORTD &= ~(1<<PORTD7 | 1<<PORTD6 | 1<<PORTD2); // 2 3 4

  // set up segments while digits off,
  // so that the full digit comes up simultanously
  // (although who is going to be able to detect a couple clk cycles?)
  segment(number);

  // then turn on specfic digit
  switch(digit)
  {
    case 1:
      PORTB |= (1<<PORTB2); // 1
      break;
    case 2:
      PORTD |= (1<<PORTD7); // 2
      break;
    case 3:
      PORTD |= (1<<PORTD6); // 3
      break;
    case 4:
      PORTD |= (1<<PORTD2); // 4
      break;
    //default:
      // bad - should't be here
  }
}

// display a number using the 7 segments
// number must be between 0-9
// TODO: add range checks
// TODO: should we ever use the decimal point?
void segment(int number)
{
  // first, reset all segments
  PORTB &= ~(1<<PORTB1 | 1<<PORTB0); // A F
  PORTC &= ~(1<<PORTC3 | 1<<PORTC2); // D E
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
      // A B D E G
      PORTB |= (1<<PORTB1); // A
      PORTC |= (1<<PORTC3 | 1<<PORTC2); // D E
      PORTD |= (1<<PORTD5 | 1<<PORTD1); // B G
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
    //default:
      // bad - should't be here
  }
}
