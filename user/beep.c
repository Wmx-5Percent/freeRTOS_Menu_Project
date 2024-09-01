#include "main.h"

void beep_control(uint8_t beep_status)
{
	if(beep_status == ON)
        PFout(8) = 1;
	else if(beep_status == OFF)
        PFout(8) = 0;
}

void beep_on_and_off()
{
    PFout(8) = 1;
	delay_ms(100);
    PFout(8) = 0;
	delay_ms(100);
}

// 长响
void beep_short()
{
	uint32_t t = 0;
	while (t++ < 500)
	{
        PFout(8) = 1;
		delay_ms(1);
	}
    PFout(8) = 0;
}

// 短响
void beep_long()
{
	uint32_t t = 0;
	while (t++ < 1000)
	{
        PFout(8) = 1;
		delay_ms(1);
	}
    PFout(8) = 0;
}
