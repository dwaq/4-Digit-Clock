#ifndef display_h
#define display_h

#ifdef __cplusplus
extern "C" {
#endif

// For LEDs:
void ledSetup(void);
void ledAm(void);
void ledPm(void);

// For 7 segment display:
void displaySetup(void);
void displayDigits(int dig1, int dig2, int dp, int dig3, int dig4);
void setDigit(int digit, int number);
void setSegment(int number);

#ifdef __cplusplus
}
#endif

#endif
