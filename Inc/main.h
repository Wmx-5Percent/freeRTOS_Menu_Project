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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

#include "FreeRTOS.h"
#include "task.h"
#include "event_groups.h"
#include "semphr.h"

#include "variables.h"
#include "spi_tft.h"
#include "spi_tft_font.h"
#include "menu.h"
#include "dht11.h"
#include "delay.h"
#include "system.h"
#include "beep.h"
#include "flash_function.h"
#include "Matrix_KeyBoard.h"
#include "password.h"
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define KEY1_Pin GPIO_PIN_2
#define KEY1_GPIO_Port GPIOE
#define KEY1_EXTI_IRQn EXTI2_IRQn
#define KEY2_Pin GPIO_PIN_3
#define KEY2_GPIO_Port GPIOE
#define KEY2_EXTI_IRQn EXTI3_IRQn
#define KEY3_Pin GPIO_PIN_4
#define KEY3_GPIO_Port GPIOE
#define KEY3_EXTI_IRQn EXTI4_IRQn
#define KB_ROW3_Pin GPIO_PIN_6
#define KB_ROW3_GPIO_Port GPIOE
#define BEEP_Pin GPIO_PIN_8
#define BEEP_GPIO_Port GPIOF
#define LED0_Pin GPIO_PIN_9
#define LED0_GPIO_Port GPIOF
#define LED1_Pin GPIO_PIN_10
#define LED1_GPIO_Port GPIOF
#define KEY0_Pin GPIO_PIN_0
#define KEY0_GPIO_Port GPIOA
#define KEY0_EXTI_IRQn EXTI0_IRQn
#define KB_COL3_Pin GPIO_PIN_4
#define KB_COL3_GPIO_Port GPIOA
#define ADC1_IN5_Pin GPIO_PIN_5
#define ADC1_IN5_GPIO_Port GPIOA
#define LED2_Pin GPIO_PIN_13
#define LED2_GPIO_Port GPIOE
#define LED3_Pin GPIO_PIN_14
#define LED3_GPIO_Port GPIOE
#define SPI_RST_Pin GPIO_PIN_6
#define SPI_RST_GPIO_Port GPIOC
#define KB_COL1_Pin GPIO_PIN_7
#define KB_COL1_GPIO_Port GPIOC
#define SPI_DC_Pin GPIO_PIN_8
#define SPI_DC_GPIO_Port GPIOC
#define KB_ROW1_Pin GPIO_PIN_9
#define KB_ROW1_GPIO_Port GPIOC
#define KB_ROW4_Pin GPIO_PIN_8
#define KB_ROW4_GPIO_Port GPIOA
#define DHT11_Pin GPIO_PIN_9
#define DHT11_GPIO_Port GPIOG
#define KB_COL2_Pin GPIO_PIN_15
#define KB_COL2_GPIO_Port GPIOG
#define KB_ROW2_Pin GPIO_PIN_6
#define KB_ROW2_GPIO_Port GPIOB
#define KB_COL4_Pin GPIO_PIN_7
#define KB_COL4_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */
#define EVENT_GROUP_RTC_WKU 0x01
#define EVENT_GROUP_BAT_EVT 0x01
#define EVENT_GROUP_TEMP_HUMI_EVT 0x01
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
