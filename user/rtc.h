#ifndef __RTC_H__
#define __RTC_H__

#include "main.h"

#define SET_TIME   1
#define UNSET_TIME 2

extern RTC_TimeTypeDef  RTC_TimeStructure;
extern RTC_DateTypeDef  RTC_DateStructure;
extern RTC_HandleTypeDef hrtc;

extern uint8_t g_rtc_wakeup_event;
extern uint8_t g_adc_conver_event;
extern uint8_t g_dht11_get_event;

extern void MX_RTC_Init(void);
extern uint8_t BCD_to_Dec(uint8_t bcd);

#endif
