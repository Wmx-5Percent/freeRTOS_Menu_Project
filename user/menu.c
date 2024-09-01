#include "main.h"

extern SemaphoreHandle_t g_mutex_printf; 

extern TaskHandle_t app_task_rtc_handle;
extern TaskHandle_t app_task_bat_handle;
extern TaskHandle_t app_task_temp_humi_handle;
extern TaskHandle_t app_task_keyboard_handle;

extern QueueHandle_t g_queue_tft_info;
extern uint8_t freertos_input_show_index;   // 计算密码在屏幕显示的长度
extern uint8_t freertos_password_index;
extern bool authenticated;
extern char freertos_client_password[PASSWORD_LENGTH + 1];

Menu_table menu_L1[];
Menu_table func0[];
Menu_table func1[];
Menu_table func2[];
Menu_table func4[];

uint8_t clear_screen_flag = 0;

Menu_table menu_L1[] = 
{
    {(uint8_t *)"Date & Time", NULL, func0, NULL, MENU_L1},
    {(uint8_t *)"Battery", NULL, func1, NULL, MENU_L1},
    {(uint8_t *)"Temp & Humi", NULL, func2, NULL, MENU_L1},
    {(uint8_t *)"RFID Mode", NULL, NULL, NULL, MENU_L1},
    {(uint8_t *)"Keyboard", NULL, func4, NULL, MENU_L1},
    {(uint8_t *)"FingerPrint", NULL, NULL, NULL, MENU_L1},
    {NULL, NULL, NULL, NULL, NULL},
};

Menu_table func0[] = 
{
    {NULL, menu_L1, NULL, rtc_handle, MENU_FUNC},
};

Menu_table func1[] = 
{
    {NULL, menu_L1, NULL, bat_handle, MENU_FUNC},
};

Menu_table func2[] = 
{
    {NULL, menu_L1, NULL, temp_humi_handle, MENU_FUNC},
};

Menu_table func4[] = 
{
    {NULL, menu_L1, NULL, keyboard_handle, MENU_FUNC},
};

void temp_humi_handle(void *Parameter)
{
    key_t *key_value = (key_t *)Parameter;

    vPortEnterCritical();
    printf("temp_humi: %d\r\n", *key_value);
    vPortExitCritical();
    if(*key_value == enter)
    {
        TFT_clear();
        vTaskResume(app_task_temp_humi_handle);
    }
    if(*key_value == back)
        vTaskSuspend(app_task_temp_humi_handle);
}

void bat_handle(void *Parameter)
{
    key_t *key_value = (key_t *)Parameter;

    vPortEnterCritical();
    printf("bat: %d\r\n", *key_value);
    vPortExitCritical();
    if(*key_value == enter)
    {
        TFT_clear();
        vTaskResume(app_task_bat_handle);
    }
    if(*key_value == back)
        vTaskSuspend(app_task_bat_handle);
}

void rtc_handle(void *Parameter)
{
    key_t *key_value = (key_t *)Parameter;

    vPortEnterCritical();
    printf("rtc: %d\r\n", *key_value);
    vPortExitCritical();
    if(*key_value == enter)
    {
        TFT_clear();
        vTaskResume(app_task_rtc_handle);
    }
    if(*key_value == back)
        vTaskSuspend(app_task_rtc_handle);
}

void keyboard_handle(void *Parameter)
{
    key_t *key_value = (key_t *)Parameter;
    tft_t tft;

    vPortEnterCritical();
    printf("keyboard: %d\r\n", *key_value);
    vPortExitCritical();
    if(*key_value == enter)
    {
        TFT_clear();

        /* 提示输入密码 */
        tft.x = 10;
        tft.y = 0;
        tft.size = 32;
        tft.str = (uint8_t *)"Password:";
        xQueueSend(g_queue_tft_info, &tft, 100);
        tft.x = 10;
        tft.y = 32;
        tft.size = 32;
        tft.str = (uint8_t *)"_";
        xQueueSend(g_queue_tft_info, &tft, 100);

        vTaskResume(app_task_keyboard_handle);
    }
    if(*key_value == back)
    {
        freertos_input_show_index = 0;
        freertos_password_index = 0;
        authenticated = false;
        memset(freertos_client_password, '\0', sizeof(freertos_client_password));
        vTaskSuspend(app_task_keyboard_handle);
    }
}

uint8_t BCD_to_Dec(uint8_t bcd)
{
    return ((bcd >> 4) * 10 + (bcd & 0x0F));
}
