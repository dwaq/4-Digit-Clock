#ifndef MAIN_H_
#define MAIN_H_

// settings state
// these values make the code easier
typedef enum
{
  DISPLAY = 9,
  SET_HR = 0xA,
  SET_MIN = 0xB,
  SET_CHIME = 0xC,
  SET_ALARM = 0xD,
  SET_ALARM_HR = 0xE,
  SET_ALARM_MIN = 0xF
} Settings_TypeDef;

void ledSetup(void);
void ledAm(void);
void ledPm(void);
void buttonsSetup(void);
void timer1Setup(void);
void nextSettingState(void);

#endif // MAIN_H_
