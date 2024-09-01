#ifndef __BEEP_H__
#define __BEEP_H__

#define ON  1
#define OFF 0

#include "main.h"

extern void beep_control(uint8_t beep_status);
extern void beep_on_and_off(void);
extern void beep_long(void);
extern void beep_short(void);

#endif
