#ifndef MAIN_H_
#define MAIN_H_

// settings state
typedef enum
{
  DISPLAY,
  SET_HR,
  SET_MIN,
  SET_CHIME,
  SET_ALARM,
  SET_ALARM_HR,
  SET_ALARM_MIN
} Settings_TypeDef;

void buttonsSetup(void);
void timer1Setup(void);

#endif // MAIN_H_
