/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "key.h"
#include "variables.h"
#include <cmsis_os2.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
extern RTC_HandleTypeDef hrtc;
extern RTC_TimeTypeDef  RTC_TimeStructure;
extern RTC_DateTypeDef  RTC_DateStructure;

static TaskHandle_t app_task_module_init_handle = NULL;
static TaskHandle_t app_task_tft_handle = NULL;
static TaskHandle_t app_task_key_handle = NULL;
TaskHandle_t app_task_test_handle = NULL;
TaskHandle_t app_task_rtc_handle = NULL;
TaskHandle_t app_task_bat_handle = NULL;
TaskHandle_t app_task_temp_humi_handle = NULL;
TaskHandle_t app_task_keyboard_handle = NULL;

/* semaphore variables */
SemaphoreHandle_t g_mutex_printf;
/* event group variables */
EventGroupHandle_t g_key_event_group;
EventGroupHandle_t g_rtc_event_group;
EventGroupHandle_t g_bat_event_group;
EventGroupHandle_t g_temp_humi_event_group;
EventGroupHandle_t g_keyboard_event_group;

/* queue variables */
QueueHandle_t g_queue_table_index;
QueueHandle_t g_queue_func_run;
QueueHandle_t g_queue_tft_info;

/**** 矩阵键盘相关全局变量 ****/

/* 键盘锁密码保存缓冲区 */
extern uint8_t password[PASSWORD_LENGTH + 1];// 正确的密码
bool promt_flag = false;	// 用户进入系统与否判断标志位
uint8_t freertos_input_show_index = 0;   // 计算密码在屏幕显示的长度
uint8_t freertos_password_index = 0;     // 计算输入密码的长度
char freertos_client_password[PASSWORD_LENGTH + 1];

/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
    .name = "defaultTask",
    .stack_size = 128 * 4,
    .priority = (osPriority_t) osPriorityNormal,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* task 01 */
static void app_task_module_init(void *pvParameter);

/* task 02 */
static void app_task_tft(void* pvParameter);

/* task 03 */
static void app_task_key(void* pvParameter);

/* task 04 */
static void app_task_rtc(void* pvParameter);

/* task 05 */
static void app_task_bat(void *pvParameter);

/* task 06 */
static void app_task_temp_humi(void *pvParameter);

/* task 07 */
static void app_task_keyboard(void *pvParameter);

/* test task */
static void app_task_test(void* pvParameter);


/* tasks variables */
static const task_t task_table[] =
{
    {app_task_tft, "app_task_tft", 256, NULL, 5, &app_task_tft_handle},
    {app_task_key, "app_task_key", 256, NULL, 5, &app_task_key_handle},
    {app_task_rtc, "app_task_rtc", 256, NULL, 5, &app_task_rtc_handle},
    {app_task_bat, "app_task_bat", 256, NULL, 5, &app_task_bat_handle},
    {app_task_temp_humi, "app_task_temp_humi", 256, NULL, 5, &app_task_temp_humi_handle},
    {app_task_keyboard, "app_task_keyboard", 512, NULL, 5, &app_task_keyboard_handle},
    // {app_task_test, "app_task_test", 512, NULL, 5, &app_task_test_handle},
    {0, 0, 0, 0, 0, 0}
};

void dgb_printf_safe(const char *format, ...)
{
#if DEBUG_PRINTF_EN

    va_list args;
    va_start(args, format);

    /* 获取互斥信号量 */
    xSemaphoreTake(g_mutex_printf, portMAX_DELAY);

    vprintf(format, args);

    /* 释放互斥信号量 */
    xSemaphoreGive(g_mutex_printf);

    va_end(args);
#else
    (void)0;
#endif
}

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
    /* USER CODE BEGIN Init */
    /* USER CODE END Init */

    /* USER CODE BEGIN RTOS_MUTEX */
    /* add mutexes, ... */
    g_mutex_printf = xSemaphoreCreateMutex();
    /* USER CODE END RTOS_MUTEX */

    /* USER CODE BEGIN RTOS_SEMAPHORES */
    /* add semaphores, ... */
    /* USER CODE END RTOS_SEMAPHORES */

    /* USER CODE BEGIN RTOS_TIMERS */
    /* start timers, add new ones, ... */
    /* USER CODE END RTOS_TIMERS */

    /* USER CODE BEGIN RTOS_QUEUES */
    /* add queues, ... */
    g_queue_table_index = xQueueCreate(5, sizeof(uint8_t));
    g_queue_func_run = xQueueCreate(5, sizeof(uint8_t));
    g_queue_tft_info = xQueueCreate(10, 30 * sizeof(uint8_t));
    /* USER CODE END RTOS_QUEUES */

    /* Create the thread(s) */
    /* creation of defaultTask */
    defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

    /* USER CODE BEGIN RTOS_THREADS */
    /* add threads, ... */
    xTaskCreate((TaskFunction_t)app_task_module_init,
                (const char*)   "app_task_module_init",
                (uint16_t)      512,
                (void *)        NULL,
                (UBaseType_t)   0,
                (TaskHandle_t *)&app_task_module_init_handle);
    /* USER CODE END RTOS_THREADS */

    /* USER CODE BEGIN RTOS_EVENTS */
    /* add events, ... */
    g_key_event_group = xEventGroupCreate();
    g_rtc_event_group = xEventGroupCreate();
    g_bat_event_group = xEventGroupCreate();
    g_temp_humi_event_group = xEventGroupCreate();
    g_keyboard_event_group = xEventGroupCreate();
    /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
    /* USER CODE BEGIN StartDefaultTask */
    /* Infinite loop */
    for(;;)
    {
        vTaskDelay(500);
        HAL_GPIO_TogglePin(LED0_GPIO_Port, LED0_Pin);
    }
    /* USER CODE END StartDefaultTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
static void app_task_module_init(void *pvParameter)
{
    uint8_t i;
    uint32_t flash_flag_read_buf[1];
    uint32_t temp_password[PASSWORD_LENGTH + 1];

    TFT_init();
    TFT_clear();

    // 检测出厂密码设置标志位
    flash_read_data(FACTORY_PASSWORD_FLAG_ADDR, flash_flag_read_buf, 1);
    printf("flash_flag_buf: %c\r\n", flash_flag_read_buf[0]);

    // 若不等于出厂设置标志位则写入出厂密码，并写入出厂设置标志位
    if(flash_flag_read_buf[0] != 'A')
    {
        printf("factory mode\r\n");
        factory_password_init();
        printf("factory mode initiate done\r\n");
    }
    // 读取密码
    else
    {
        flash_read_data(FLASH_USER_START_ADDR, (uint32_t *)temp_password, PASSWORD_LENGTH + 1);
        // hex_to_string((const char *)temp_password, (char *)password, PASSWORD_LENGTH + 1);
        hex_to_string(temp_password, (char *)password, sizeof(temp_password) / sizeof(temp_password[0]));
    }

    vPortEnterCritical();
    i = 0;

    /* 循环创建任务 */
    while(task_table[i].usStackDepth != 0)
    {
        xTaskCreate(
            task_table[i].pxTaskCode,
            task_table[i].pcName,
            task_table[i].usStackDepth,
            task_table[i].pvParameters,
            task_table[i].uxPriority,
            task_table[i].pxCreatedTask
        );

        i++;
    }
    vPortExitCritical();

    /* 挂起功能性函数 */
    vTaskSuspend(app_task_rtc_handle);
    vTaskSuspend(app_task_bat_handle);
    vTaskSuspend(app_task_temp_humi_handle);
    vTaskSuspend(app_task_keyboard_handle);

    vTaskDelete(NULL);
}

/* 计算菜单现有的条目 */
/* @param: table 传递菜单结构体变量 */
/* @return value: 菜单条目数量 - 1，因为数组下标从零开始算 */
uint8_t total_menu_items(Menu_table *table)
{
    Menu_table *ptable = table;

    uint8_t total_items = 0;

    while (ptable->item != NULL)
    {
        ptable++;

        total_items++;
    }
    return (total_items - 1);
}

/* tft屏幕显示菜单条目 */
/* @param: menu 传递菜单结构体变量 */
void menu_show(Menu_table *menu)
{
    Menu_table *pmenu;
    tft_t tft;
    Menu_table menu_copy;

    uint8_t i = 0;

    pmenu = menu;

    while(pmenu->item != NULL)
    {
        /* 拷贝菜单结构体数组元素，方式信息传递时指针操作越界 */
        menu_copy = *pmenu;

        /* 拷贝屏幕显示参数，并发送队列信息 */
        tft.str = menu_copy.item;
        tft.x = 60;
        tft.y = i * 32;
        tft.size = 32;

        // dgb_printf_safe("%s\r\n", menu_copy.item);
        xQueueSend(g_queue_tft_info, &tft, 100);
        pmenu++;
        i++;

        vTaskDelay(1);
    }
}

/* 通过接收来自按键外部中断的事件组来操作菜单 */
static void app_task_key(void *pvParameter)
{
    EventBits_t uxKeyBits;

    uint8_t table_index = 0;
    uint8_t total_items = 0;
    uint8_t func_run = 0;

    Menu_table *current_menu_L1 = menu_L1;
    Menu_table *m = current_menu_L1;

    key_t key_func;

    tft_t tft;

    // dgb_printf_safe("key task\r\n");

    /* 屏幕初始化菜单显 */
    tft.x = 10;
    tft.y = table_index * 32;
    tft.size = 32;
    tft.str = (uint8_t *)"->";
    xQueueSend(g_queue_tft_info, &tft, 100);
    menu_show(m);

    for(;;)
    {
        // vTaskDelay(500);
#if 1
        uxKeyBits = xEventGroupWaitBits(g_key_event_group,
                                        KEY_DOWN|KEY_UP|KEY_ENTER|KEY_BACK,
                                        pdTRUE,
                                        pdFALSE,
                                        portMAX_DELAY);
        if(uxKeyBits & KEY_UP)
        {
            /* 若有功能函数正在运行 */
            if(func_run)
                continue;

            /* 消除原来的箭头 */
            tft.x = 10;
            tft.y = table_index * 32;
            tft.size = 32;
            tft.str = (uint8_t *)"  ";
            xQueueSend(g_queue_tft_info, &tft, 100);

            /* 若table_index没有减至0 */
            if(table_index)
            {
                m--;
                table_index--;
            }
            else
            {
                /* 若table_index减至0，则指向菜单中最后一个条目 */
                total_items = total_menu_items(m);
                table_index = total_items;
                m += table_index;
            }

            dgb_printf_safe("table_index: %d\r\n", table_index);

            /* 显示新箭头位置 */
            tft.x = 10;
            tft.y = table_index * 32;
            tft.size = 32;
            tft.str = (uint8_t *)"->";
            xQueueSend(g_queue_tft_info, &tft, 100);
        }
        else if(uxKeyBits & KEY_DOWN)
        {
            /* 若有功能函数正在运行 */
            if(func_run)
                continue;

            /* 消除原来的箭头 */
            tft.x = 10;
            tft.y = table_index * 32;
            tft.size = 32;
            tft.str = (uint8_t *)"  ";
            xQueueSend(g_queue_tft_info, &tft, 100);

            /* 判断目前在几级菜单 */
            /* 菜单总数的计算需要传递菜单结构体的首地址 */
            if(m->menu_type == MENU_L1)
                total_items = total_menu_items(current_menu_L1);

            dgb_printf_safe("[key]:total_items:%d\r\n", total_items);

            /* 判断是否超出该级菜单条目总数 */
            if(table_index >= total_items)
            {
                /* 菜单结构体指针重新指向数组首元素 */
                m -= table_index;
                table_index = 0;
            }
            else
            {
                m++;
                table_index++;
            }

            dgb_printf_safe("table_index: %d\r\n", table_index);

            /* 显示新箭头位置 */
            tft.x = 10;
            tft.y = table_index * 32;
            tft.size = 32;
            tft.str = (uint8_t *)"->";
            xQueueSend(g_queue_tft_info, &tft, 100);
        }
        else if(uxKeyBits & KEY_ENTER)
        {
            /* 若有功能函数正在运行 */
            if(func_run)
                continue;

            /* 记录按键信息，用于功能函数判断需要恢复还是挂起任务 */
            key_func = enter;

            /* 判断是否有菜单子条目 */
            if(m->child)
            {
                m = m->child;
                menu_show(m);
            }

            /* 若无子条目，且有功能函数 */
            if((m->child == NULL) && (m->func))
            {
                func_run = 1;
                m->func(&key_func); // 执行功能函数，传递按键信息
                dgb_printf_safe("[enter]:func_run:%d\r\n", func_run);
            }

        }
        else if(uxKeyBits & KEY_BACK)
        {
            key_func = back;

            /* 若有功能函数则运行功能函数，传递按键信息，挂起任务 */
            if(func_run)
            {
                m->func(&key_func);
            }

            /* 若有上一级菜单，返回上一级菜单 */
            if(m->parent)
            {
                m = m->parent + table_index;

                TFT_clear();

                func_run = 0;

                /* 显示新箭头位置 */
                tft.x = 10;
                tft.y = table_index * 32;
                tft.size = 32;
                tft.str = (uint8_t *)"->";
                xQueueSend(g_queue_tft_info, &tft, 100);

                if(m->menu_type == MENU_L1)
                    menu_show(current_menu_L1);
            }
        }
#endif
    }
}

/* tft 显示任务 */
/* 接收tft消息，显示相应文字 */
static void app_task_tft(void *pvParameter)
{
    BaseType_t xReturn;

    tft_t tft;

    for(;;)
    {
        // dgb_printf_safe("[tft]\r\n");
        xReturn = xQueueReceive(g_queue_tft_info,
                                &tft,
                                portMAX_DELAY);


        if(xReturn == pdFALSE)
        {
            dgb_printf_safe("[tft]: received false\r\n");
            continue;
        }
        else
        {
            // dgb_printf_safe("[tft]:%s\r\n", tft.str);
            // dgb_printf_safe("[tft]:x,%d y,%d, size,%d\r\n", tft.x, tft.y, tft.size);
            show_char(tft.x, tft.y, BLACK, tft.str, tft.size);
        }

    }
}

/* 日期时间任务，使用RTC实现 */
static void app_task_rtc(void *pvParameter)
{
    EventBits_t uxRTCBits;

    tft_t tft;

    char yymmdd_str[20];
    char hhmmss_str[20];

    for(;;)
    {
        /* 等待来自RTC唤醒中断发来的事件 */
        uxRTCBits = xEventGroupWaitBits(g_rtc_event_group,
                                        EVENT_GROUP_RTC_WKU,
                                        pdTRUE,
                                        pdFALSE,
                                        portMAX_DELAY);
        if(uxRTCBits == EVENT_GROUP_RTC_WKU)
        {
            dgb_printf_safe("rtc task\r\n");

            // HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);
            HAL_RTC_GetTime(&hrtc, &RTC_TimeStructure, RTC_FORMAT_BCD);
            sprintf(hhmmss_str, "%02d:%02d:%02d",
                    BCD_to_Dec(RTC_TimeStructure.Hours),
                    BCD_to_Dec(RTC_TimeStructure.Minutes),
                    BCD_to_Dec(RTC_TimeStructure.Seconds));

            HAL_RTC_GetDate(&hrtc, &RTC_DateStructure, RTC_FORMAT_BCD);
            sprintf(yymmdd_str, "20%02d-%02d-%02d",
                    BCD_to_Dec(RTC_DateStructure.Year),
                    BCD_to_Dec(RTC_DateStructure.Month),
                    BCD_to_Dec(RTC_DateStructure.Date));

            /* 日期 */
            tft.x = 10;
            tft.y = 0;
            tft.str = (uint8_t *)"Date:";
            tft.size = 32;
            xQueueSend(g_queue_tft_info, &tft, 100);
            tft.x = 10;
            tft.y = 32;
            tft.str = (uint8_t *)yymmdd_str;
            tft.size = 32;
            /* 时间 */
            xQueueSend(g_queue_tft_info, &tft, 100);
            tft.x = 10;
            tft.y = 64;
            tft.str = (uint8_t *)"Time:";
            tft.size = 32;
            xQueueSend(g_queue_tft_info, &tft, 100);
            tft.x = 10;
            tft.y = 96;
            tft.str = (uint8_t *)hhmmss_str;
            tft.size = 32;
            xQueueSend(g_queue_tft_info, &tft, 100);
            // printf("20%02x/%02x/%02x week:%d\r\n",RTC_DateStructure.Year, RTC_DateStructure.Month, RTC_DateStructure.Date,RTC_DateStructure.WeekDay);
            // printf("%02x:%02x:%02x\r\n",RTC_TimeStructure.Hours, RTC_TimeStructure.Minutes, RTC_TimeStructure.Seconds);
        }
    }
}

/* 电池电量检测任务 */
static void app_task_bat(void *pvParameter)
{
    EventBits_t uxBatBits;

    tft_t tft;

    uint8_t clear_screen_flag = 0;
    uint16_t vol_percent;
    double adc_val, adc_vol;
    char battery_str[5];

    extern ADC_HandleTypeDef hadc1;

    dgb_printf_safe("[task_bat]\r\n");

    for(;;)
    {
        // dgb_printf_safe("[task_bat]\r\n");
        /* 等待来自RTC唤醒中断的事件组，每1秒检测一次 */
        uxBatBits = xEventGroupWaitBits(g_bat_event_group,
                                        EVENT_GROUP_BAT_EVT,
                                        pdTRUE,
                                        pdFALSE,
                                        portMAX_DELAY);
        if(uxBatBits == EVENT_GROUP_BAT_EVT)
        {
            dgb_printf_safe("bat\r\n");

            /* 等待ADC转换完成，清楚转换完成标志位 */
            while(__HAL_ADC_GET_FLAG(&hadc1, ADC_FLAG_EOC) == RESET);
            __HAL_ADC_CLEAR_FLAG(&hadc1, ADC_FLAG_EOC);
            adc_val=HAL_ADC_GetValue(&hadc1);
            adc_vol = (adc_val * 3300) / 4095 / 1000;
            vol_percent = (adc_vol / 3.3) * 100;

            sprintf(battery_str, "%d%%", vol_percent);

            /* 只有当数值位数发生变化才进行清屏操作，减少tft刷新频率 */
            if(vol_percent < 10 && clear_screen_flag == 0)
            {
                clear_screen_flag = 1;
                TFT_clear();
            }
            else if(vol_percent >= 10)
                clear_screen_flag = 0;

            tft.x = 10;
            tft.y = 0;
            tft.str = (uint8_t *)"Battery:";
            tft.size = 32;
            xQueueSend(g_queue_tft_info, &tft, 100);
            tft.x = 10;
            tft.y = 32;
            tft.str = (uint8_t *)battery_str;
            tft.size = 32;
            xQueueSend(g_queue_tft_info, &tft, 100);

            // printf("adc_val: %.f\r\n", adc_val);
            // printf("adc_vol: %.2f\r\n", adc_vol);
            // printf("vol per: %d%%\r\n", vol_percent);
        }
    }
}

/* 温湿度检测任务 */
static void app_task_temp_humi(void *pvParameter)
{
    EventBits_t uxTempHumiBits;

    tft_t tft;

    uint8_t dht11_buf[5];
    int8_t dht11_rt;
    uint8_t temp_str[10];
    uint8_t humi_str[10];

    for(;;)
    {
        dgb_printf_safe("temp_humi\r\n");
        /* 等待来自RTC唤醒中断的事件组，每1秒检测一次 */
        uxTempHumiBits = xEventGroupWaitBits(g_temp_humi_event_group,
                                             EVENT_GROUP_TEMP_HUMI_EVT,
                                             pdTRUE,
                                             pdFALSE,
                                             portMAX_DELAY);

        if(uxTempHumiBits == EVENT_GROUP_TEMP_HUMI_EVT)
        {
            dht11_rt = dht11_get_data(dht11_buf);
            delay_ms(10);
            if(dht11_rt < 0)
            {
                tft.x = 10;
                tft.y = 0;
                tft.str = (uint8_t *)"ERROR";
                tft.size = 32;
                xQueueSend(g_queue_tft_info, &tft, 100);
            }
            else
            {
                sprintf((char *)temp_str, "%d.%dC", dht11_buf[2], dht11_buf[3]);
                sprintf((char *)humi_str, "%d.%d%%", dht11_buf[0], dht11_buf[1]);
                tft.x = 10;
                tft.y = 0;
                tft.str = (uint8_t *)temp_str;
                tft.size = 32;
                xQueueSend(g_queue_tft_info, &tft, 100);
                tft.x = 10;
                tft.y = 32;
                tft.str = (uint8_t *)humi_str;
                tft.size = 32;
                xQueueSend(g_queue_tft_info, &tft, 100);
                // show_char(10, 0, BLACK, temp_str, 32);
                // show_char(10, 32, BLACK, humi_str, 32);
            }
        }
    }
}

static void app_task_keyboard(void *pvParameter)
{
    int8_t key_val = 0;
    char convertion_val;

    tft_t tft;

    BaseType_t uxHighWaterMark;

    for(;;)
    {
        key_val = matrix_key_scan();

        // dgb_printf_safe("keyboard\r\n");

        // 判断是否输入正确密码进入系统
        if(authenticated)
        {
            // 开始扫描用户按键，执行相应的功能
            if(key_val >0 && key_val < 17)
            {
                convertion_val = matrix_val_convert(key_val);

                // dgb_printf_safe("authenticated:%c\r\n", convertion_val);

                uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
                dgb_printf_safe("[before]water mark:%lu\r\n", uxHighWaterMark);

                // 通过识别到的按键，进行功能选择
                handle_authenticated_input(convertion_val);

                uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
                dgb_printf_safe("[after]water mark:%lu\r\n", uxHighWaterMark);
            }
        }
        // 若为正确输入密码则开始输入密码
        else
        {
            // 打印提示信息，让用户输入密码，后期加入LED0闪烁
            if(!promt_flag)
            {
                dgb_printf_safe("please input password\r\n");
                promt_flag = true;
            }

            if(key_val >0 && key_val < 17)
            {
                // 转换键盘输入的数据
                convertion_val = matrix_val_convert(key_val);

                dgb_printf_safe("[else] val:%c, password_index:%d\r\n", convertion_val, freertos_password_index);
                tft.x = 10 + freertos_input_show_index * 32;
                tft.y = 32;
                tft.size = 32;
                tft.str = (uint8_t *)"*";
                xQueueSend(g_queue_tft_info, &tft, 100);
                freertos_input_show_index++;
                tft.x = 10 + freertos_input_show_index * 32;
                tft.y = 32;
                tft.size = 32;
                tft.str = (uint8_t *)"_";
                xQueueSend(g_queue_tft_info, &tft, 100);

                // 将转换后的密码放进密码存放缓存
                freertos_client_password[freertos_password_index++] = convertion_val;
                dgb_printf_safe("freertos_client_password:%s, password_index:%d\r\n", freertos_client_password, freertos_password_index);

                // 判断输入密码长度
                if(freertos_password_index == PASSWORD_LENGTH || freertos_password_index == FAKE_PASSWORD_LENGTH)
                {
                    freertos_client_password[freertos_password_index] = '\0';
                    // test_password[freertos_password_index] = '\0';

                    freertos_password_index = 0;
                    freertos_input_show_index = 0;

                    // 验证密码正确性
                    verify_password((uint8_t *)freertos_client_password, password, 1);

                    memset(freertos_client_password, '\0', sizeof(freertos_client_password));
                }
            }  
        }


        vTaskDelay(1);
    }
}

#if 1
static void app_task_test(void *pvParameter)
{
    for(;;)
    {
        dgb_printf_safe("[test_task]\r\n");

        vTaskDelay(1000);
    }
}
#endif
/* USER CODE END Application */

