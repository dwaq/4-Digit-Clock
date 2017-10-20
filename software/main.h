#ifndef main_h
#define main_h

// These values make the code easier, because you can
// use the same values for display during the setting mode
// and they're sequential so you can increment through them
// Need to be defines for the switch statement to work
// settings states
#define CLOCK 9
#define SET_HR 0xA
#define SET_MIN 0xB
#define SET_CHIME 0xC
#define SET_ALARM 0xD
#define SET_ALARM_HR 0xE
#define SET_ALARM_MIN 0xF
// letter n for displaying "on"
#define N 0x10
// special parameters to handle decimal point
#define DP_ON 0x11
// this is a general parameter to turn any segment off
#define BLANK 0x12

#ifdef __cplusplus
extern "C" {
#endif

// For buttons:
void buttonsSetup(void);
void buttonBuzzer(void);
void buttonsS1(void);
void buttonsS2(void);

// For moving through settings:
void nextSettingState(void);

// For increasing time:
void increaseSecond(void);
void increaseHour(void);

#ifdef __cplusplus
}
#endif

#endif
