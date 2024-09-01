/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32f4xx_it.c
  * @brief   Interrupt Service Routines.
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
#include "main.h"
#include "stm32f4xx_it.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */
uint8_t g_rtc_wakeup_event = 0;
uint8_t g_adc_conver_event = 0;
uint8_t g_dht11_get_event  = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
extern uint8_t g_key_result;

extern EventGroupHandle_t g_key_event_group;
extern SemaphoreHandle_t g_mutex_printf;
extern EventGroupHandle_t g_rtc_event_group;
extern EventGroupHandle_t g_bat_event_group;
extern EventGroupHandle_t g_temp_humi_event_group;
/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/
extern RTC_HandleTypeDef hrtc;
extern DMA_HandleTypeDef hdma_spi1_tx;
extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;
extern UART_HandleTypeDef huart1;
extern TIM_HandleTypeDef htim4;

/* USER CODE BEGIN EV */

/* USER CODE END EV */

/******************************************************************************/
/*           Cortex-M4 Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void)
{
    /* USER CODE BEGIN NonMaskableInt_IRQn 0 */

    /* USER CODE END NonMaskableInt_IRQn 0 */
    /* USER CODE BEGIN NonMaskableInt_IRQn 1 */
    while (1)
    {
    }
    /* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void)
{
    /* USER CODE BEGIN HardFault_IRQn 0 */

    /* USER CODE END HardFault_IRQn 0 */
    while (1)
    {
        /* USER CODE BEGIN W1_HardFault_IRQn 0 */
        /* USER CODE END W1_HardFault_IRQn 0 */
    }
}

/**
  * @brief This function handles Memory management fault.
  */
void MemManage_Handler(void)
{
    /* USER CODE BEGIN MemoryManagement_IRQn 0 */

    /* USER CODE END MemoryManagement_IRQn 0 */
    while (1)
    {
        /* USER CODE BEGIN W1_MemoryManagement_IRQn 0 */
        /* USER CODE END W1_MemoryManagement_IRQn 0 */
    }
}

/**
  * @brief This function handles Pre-fetch fault, memory access fault.
  */
void BusFault_Handler(void)
{
    /* USER CODE BEGIN BusFault_IRQn 0 */

    /* USER CODE END BusFault_IRQn 0 */
    while (1)
    {
        /* USER CODE BEGIN W1_BusFault_IRQn 0 */
        /* USER CODE END W1_BusFault_IRQn 0 */
    }
}

/**
  * @brief This function handles Undefined instruction or illegal state.
  */
void UsageFault_Handler(void)
{
    /* USER CODE BEGIN UsageFault_IRQn 0 */

    /* USER CODE END UsageFault_IRQn 0 */
    while (1)
    {
        /* USER CODE BEGIN W1_UsageFault_IRQn 0 */
        /* USER CODE END W1_UsageFault_IRQn 0 */
    }
}

/**
  * @brief This function handles Debug monitor.
  */
void DebugMon_Handler(void)
{
    /* USER CODE BEGIN DebugMonitor_IRQn 0 */

    /* USER CODE END DebugMonitor_IRQn 0 */
    /* USER CODE BEGIN DebugMonitor_IRQn 1 */

    /* USER CODE END DebugMonitor_IRQn 1 */
}

/******************************************************************************/
/* STM32F4xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f4xx.s).                    */
/******************************************************************************/

/**
  * @brief This function handles RTC wake-up interrupt through EXTI line 22.
  */
void RTC_WKUP_IRQHandler(void)
{
    /* USER CODE BEGIN RTC_WKUP_IRQn 0 */

    /* USER CODE END RTC_WKUP_IRQn 0 */
    HAL_RTCEx_WakeUpTimerIRQHandler(&hrtc);
    /* USER CODE BEGIN RTC_WKUP_IRQn 1 */

    /* USER CODE END RTC_WKUP_IRQn 1 */
}

/**
  * @brief This function handles EXTI line0 interrupt.
  */
void EXTI0_IRQHandler(void)
{
    /* USER CODE BEGIN EXTI0_IRQn 0 */

    /* USER CODE END EXTI0_IRQn 0 */
    HAL_GPIO_EXTI_IRQHandler(KEY0_Pin);
    /* USER CODE BEGIN EXTI0_IRQn 1 */

    /* USER CODE END EXTI0_IRQn 1 */
}

/**
  * @brief This function handles EXTI line2 interrupt.
  */
void EXTI2_IRQHandler(void)
{
    /* USER CODE BEGIN EXTI2_IRQn 0 */

    /* USER CODE END EXTI2_IRQn 0 */
    HAL_GPIO_EXTI_IRQHandler(KEY1_Pin);
    /* USER CODE BEGIN EXTI2_IRQn 1 */

    /* USER CODE END EXTI2_IRQn 1 */
}

/**
  * @brief This function handles EXTI line3 interrupt.
  */
void EXTI3_IRQHandler(void)
{
    /* USER CODE BEGIN EXTI3_IRQn 0 */

    /* USER CODE END EXTI3_IRQn 0 */
    HAL_GPIO_EXTI_IRQHandler(KEY2_Pin);
    /* USER CODE BEGIN EXTI3_IRQn 1 */

    /* USER CODE END EXTI3_IRQn 1 */
}

/**
  * @brief This function handles EXTI line4 interrupt.
  */
void EXTI4_IRQHandler(void)
{
    /* USER CODE BEGIN EXTI4_IRQn 0 */

    /* USER CODE END EXTI4_IRQn 0 */
    HAL_GPIO_EXTI_IRQHandler(KEY3_Pin);
    /* USER CODE BEGIN EXTI4_IRQn 1 */

    /* USER CODE END EXTI4_IRQn 1 */
}

/**
  * @brief This function handles TIM1 update interrupt and TIM10 global interrupt.
  */
void TIM1_UP_TIM10_IRQHandler(void)
{
    /* USER CODE BEGIN TIM1_UP_TIM10_IRQn 0 */

    /* USER CODE END TIM1_UP_TIM10_IRQn 0 */
    HAL_TIM_IRQHandler(&htim1);
    /* USER CODE BEGIN TIM1_UP_TIM10_IRQn 1 */

    /* USER CODE END TIM1_UP_TIM10_IRQn 1 */
}

/**
  * @brief This function handles TIM2 global interrupt.
  */
void TIM2_IRQHandler(void)
{
    /* USER CODE BEGIN TIM2_IRQn 0 */

    /* USER CODE END TIM2_IRQn 0 */
    HAL_TIM_IRQHandler(&htim2);
    /* USER CODE BEGIN TIM2_IRQn 1 */

    /* USER CODE END TIM2_IRQn 1 */
}

/**
  * @brief This function handles TIM3 global interrupt.
  */
void TIM3_IRQHandler(void)
{
    /* USER CODE BEGIN TIM3_IRQn 0 */

    /* USER CODE END TIM3_IRQn 0 */
    HAL_TIM_IRQHandler(&htim3);
    /* USER CODE BEGIN TIM3_IRQn 1 */

    /* USER CODE END TIM3_IRQn 1 */
}

/**
  * @brief This function handles TIM4 global interrupt.
  */
void TIM4_IRQHandler(void)
{
    /* USER CODE BEGIN TIM4_IRQn 0 */

    /* USER CODE END TIM4_IRQn 0 */
    HAL_TIM_IRQHandler(&htim4);
    /* USER CODE BEGIN TIM4_IRQn 1 */

    /* USER CODE END TIM4_IRQn 1 */
}

/**
  * @brief This function handles USART1 global interrupt.
  */
void USART1_IRQHandler(void)
{
    /* USER CODE BEGIN USART1_IRQn 0 */

    /* USER CODE END USART1_IRQn 0 */
    HAL_UART_IRQHandler(&huart1);
    /* USER CODE BEGIN USART1_IRQn 1 */

    /* USER CODE END USART1_IRQn 1 */
}

/**
  * @brief This function handles DMA2 stream3 global interrupt.
  */
void DMA2_Stream3_IRQHandler(void)
{
    /* USER CODE BEGIN DMA2_Stream3_IRQn 0 */

    /* USER CODE END DMA2_Stream3_IRQn 0 */
    HAL_DMA_IRQHandler(&hdma_spi1_tx);
    /* USER CODE BEGIN DMA2_Stream3_IRQn 1 */

    /* USER CODE END DMA2_Stream3_IRQn 1 */
}

/* USER CODE BEGIN 1 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    //?1?7??1?7?1?7EXTI0?1?7?0?9?1?7?1?7?1?7?1?7閿熸枻锟�???1?7?1?7?1?7?1?7?
    if(GPIO_Pin == KEY0_Pin)
    {
        __HAL_GPIO_EXTI_CLEAR_IT(KEY0_Pin);

        xSemaphoreTakeFromISR(g_mutex_printf, NULL);
        printf("KEY0 IT\r\n");
        xSemaphoreGiveFromISR(g_mutex_printf, NULL);

        /* ?1?7?1?9?0?2?1?7?0?2?1?7?1 */
        HAL_TIM_Base_Stop_IT(&htim1);

        /* ?1?7?1?7?0?1?1?7?0?2?1?7?1?7?1?7?1?7???1?7? */
        __HAL_TIM_SetCounter(&htim1, 0);

        /* ?1?7?1?7?1?7?1?7?1?7?1?7?0?2?1?7?1?1?7?1?7?1?7?1?7?1?7?1?7?1?7?1?7?1?7?閿熸枻锟�???1?7?0?8?1?7?1?7?1?7?1?7?1?3?1?7?0?6?1?7?1?7?1?7?1?7?1?7?0?2?1?7?0?2?1?7? */
        HAL_TIM_Base_Start_IT(&htim1);
    }
    else if(GPIO_Pin == KEY1_Pin)
    {
        printf("KEY1 IT\r\n");

        /* ?1?7?1?9?0?2?1?7?0?2?1?7?1 */
        HAL_TIM_Base_Stop_IT(&htim1);

        /* ?1?7?1?7?0?1?1?7?0?2?1?7?1?7?1?7?1?7???1?7? */
        __HAL_TIM_SetCounter(&htim1, 0);

        /* ?1?7?1?7?1?7?1?7?1?7?1?7?0?2?1?7?1?1?7?1?7?1?7?1?7?1?7?1?7?1?7?1?7?1?7?閿熸枻锟�???1?7?0?8?1?7?1?7?1?7?1?7?1?3?1?7?0?6?1?7?1?7?1?7?1?7?1?7?0?2?1?7?0?2?1?7? */
        HAL_TIM_Base_Start_IT(&htim1);

        __HAL_GPIO_EXTI_CLEAR_IT(KEY1_Pin);
    }
    else if(GPIO_Pin == KEY2_Pin)
    {
        printf("KEY2 IT\r\n");

        /* ?1?7?1?9?0?2?1?7?0?2?1?7?1 */
        HAL_TIM_Base_Stop_IT(&htim1);

        /* ?1?7?1?7?0?1?1?7?0?2?1?7?1?7?1?7?1?7???1?7? */
        __HAL_TIM_SetCounter(&htim1, 0);

        /* ?1?7?1?7?1?7?1?7?1?7?1?7?0?2?1?7?1?1?7?1?7?1?7?1?7?1?7?1?7?1?7?1?7?1?7?閿熸枻锟�???1?7?0?8?1?7?1?7?1?7?1?7?1?3?1?7?0?6?1?7?1?7?1?7?1?7?1?7?0?2?1?7?0?2?1?7? */
        HAL_TIM_Base_Start_IT(&htim1);

        __HAL_GPIO_EXTI_CLEAR_IT(KEY2_Pin);
    }
    else if(GPIO_Pin == KEY3_Pin)
    {
        printf("KEY3 IT\r\n");

        /* ?1?7?1?9?0?2?1?7?0?2?1?7?1 */
        HAL_TIM_Base_Stop_IT(&htim1);

        /* ?1?7?1?7?0?1?1?7?0?2?1?7?1?7?1?7?1?7???1?7? */
        __HAL_TIM_SetCounter(&htim1, 0);

        /* ?1?7?1?7?1?7?1?7?1?7?1?7?0?2?1?7?1?1?7?1?7?1?7?1?7?1?7?1?7?1?7?1?7?1?7?閿熸枻锟�???1?7?0?8?1?7?1?7?1?7?1?7?1?3?1?7?0?6?1?7?1?7?1?7?1?7?1?7?0?2?1?7?0?2?1?7? */
        HAL_TIM_Base_Start_IT(&htim1);

        __HAL_GPIO_EXTI_CLEAR_IT(KEY3_Pin);
    }
}

void HAL_RTCEx_WakeUpTimerEventCallback(RTC_HandleTypeDef *hrtc)
{
    if(__HAL_RTC_WAKEUPTIMER_GET_IT(hrtc, RTC_IT_WUT) != RESET)
    {
        __HAL_RTC_WAKEUPTIMER_CLEAR_FLAG(hrtc, RTC_FLAG_WUTF);

        // xSemaphoreTakeFromISR(g_mutex_printf, NULL);
        // printf("RTC\r\n");
        // xSemaphoreGiveFromISR(g_mutex_printf, NULL);
        xEventGroupSetBitsFromISR(g_rtc_event_group, EVENT_GROUP_RTC_WKU, NULL);
        xEventGroupSetBitsFromISR(g_bat_event_group, EVENT_GROUP_BAT_EVT, NULL);
        xEventGroupSetBitsFromISR(g_temp_humi_event_group, EVENT_GROUP_TEMP_HUMI_EVT, NULL);
        // g_rtc_wakeup_event = 1;
        // g_adc_conver_event = 1;
        // g_dht11_get_event = 1;

        __HAL_RTC_EXTI_CLEAR_FLAG(RTC_EXTI_LINE_WAKEUPTIMER_EVENT);
    }
}

#if 0
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if(htim->Instance == TIM1)
    {
        if (HAL_GPIO_ReadPin(KEY0_GPIO_Port, KEY0_Pin) == GPIO_PIN_RESET)
        {
            xSemaphoreTakeFromISR(g_mutex_printf, NULL);
            printf("KEY0 press\r\n");
            xSemaphoreGiveFromISR(g_mutex_printf, NULL);
            // g_key_result = DOWN;
            xEventGroupSetBitsFromISR(g_key_event_group, 0x01, NULL);
        }
        else if (HAL_GPIO_ReadPin(KEY1_GPIO_Port, KEY1_Pin) == GPIO_PIN_RESET)
        {
            // printf("KEY1 press\r\n");
            // g_key_result = UP;
            xEventGroupSetBitsFromISR(g_key_event_group, 0x02, NULL);
        }
        else if (HAL_GPIO_ReadPin(KEY2_GPIO_Port, KEY2_Pin) == GPIO_PIN_RESET)
        {
            // printf("KEY2 press\r\n");
            // g_key_result = ENTER;
            xEventGroupSetBitsFromISR(g_key_event_group, 0x04, NULL);
        }
        else if (HAL_GPIO_ReadPin(KEY3_GPIO_Port, KEY3_Pin) == GPIO_PIN_RESET)
        {
            // printf("KEY3 press\r\n");
            // g_key3_event = 1;
            xEventGroupSetBitsFromISR(g_key_event_group, 0x08, NULL);
        }

        // 鍋滄 TIM1
        HAL_TIM_Base_Stop_IT(&htim1);

        // 閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹鍗稿閿熻锟�??
        __HAL_TIM_CLEAR_FLAG(&htim1, TIM_FLAG_UPDATE);
    }
}
#endif
/* USER CODE END 1 */
