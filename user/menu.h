#ifndef __MENU_H__
#define __MENU_H__

#include "main.h"

#define DOWN  1
#define UP    2
#define ENTER 3
#define BACK  4

#define MENU_FUNC 0
#define MENU_L1 1

extern void temp_humi_handle(void *Parameter);
extern void bat_handle(void *Parameter);
extern void rtc_handle(void *Parameter);

typedef struct __tft_t
{
    uint8_t x;
    uint8_t y;
    
    uint16_t color;

    uint8_t *str;

    uint8_t size;
} tft_t;

typedef enum __key_t
{
    none = 0,
    up,
    down,
    enter,
    back,
} key_t;

typedef struct __menu_t
{
    uint8_t *item;
    struct __menu_t *parent; 
    struct __menu_t *child; 
    void (*func)(void *parameter);
    uint8_t menu_type;
} Menu_table;

extern Menu_table menu_L1[];
extern Menu_table func0[];
extern Menu_table func1[];
extern Menu_table func2[];
extern Menu_table func4[];
extern void temp_humi_handle(void *Parameter);
extern void bat_handle(void *Parameter);
extern void rtc_handle(void *Parameter);
extern void keyboard_handle(void *Parameter);

extern uint8_t BCD_to_Dec(uint8_t bcd);


#endif
