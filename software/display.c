#include "display.h"

void display(int dig1, int dig2, int dig3, int dig4);
void digit(int digit, int segment);

// display a number using the 7 segments
// number must be between 0-9
// TODO: add range checks
void segment(int number)
{
  switch(number) {
    case 0:
      // A B C D E F

      break;
    case 1:
      // B C

      break;
    case 2:
    // A B D E G

      break;
    case 3:
      // A B D E G

      break;
    case 4:
      // B C F G

      break;
    case 5:
      // A C D F G

      break;
    case 6:
      // A C D E F G

      break;
    case 7:
      // A B C

      break;
    case 8:
      // A B C D E F G

      break;
    case 9:
      // A B C D F G

      break;
    default:
      // bad - should't be here
  }
}
