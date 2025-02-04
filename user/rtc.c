#include "rtc.h"

RTC_TimeTypeDef  RTC_TimeStructure;
RTC_DateTypeDef  RTC_DateStructure;
RTC_HandleTypeDef hrtc;

uint8_t g_rtc_wakeup_event = 0;
uint8_t g_adc_conver_event = 0;
uint8_t g_dht11_get_event  = 0;

/* RTC init function */
void MX_RTC_Init(void)
{

    /* USER CODE BEGIN RTC_Init 0 */

    /* USER CODE END RTC_Init 0 */

    RTC_TimeTypeDef sTime = {0};
    RTC_DateTypeDef sDate = {0};

    /* USER CODE BEGIN RTC_Init 1 */

    /* USER CODE END RTC_Init 1 */

    /** Initialize RTC Only
    */
    hrtc.Instance = RTC;
    hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
    hrtc.Init.AsynchPrediv = 0x7F;
    hrtc.Init.SynchPrediv = 0xFF;
    hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
    hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
    hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
    if (HAL_RTC_Init(&hrtc) != HAL_OK)
    {
        Error_Handler();
    }

    /* USER CODE BEGIN Check_RTC_BKUP */

    /* USER CODE END Check_RTC_BKUP */

    /** Initialize RTC and set the Time and Date
    */
    if(HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR0) != 0x1688)
    {
        sTime.Hours = 0x14;
        sTime.Minutes = 0x14;
        sTime.Seconds = 0x0;
        sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
        sTime.StoreOperation = RTC_STOREOPERATION_RESET;
        if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD) != HAL_OK)
        {
            Error_Handler();
        }
        sDate.WeekDay = RTC_WEEKDAY_SATURDAY;
        sDate.Month = RTC_MONTH_JULY;
        sDate.Date = 0x27;
        sDate.Year = 0x24;

        if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BCD) != HAL_OK)
        {
            Error_Handler();
        }

        HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR0, 0x1688);
    }


    /** Enable the WakeUp
    */
    if (HAL_RTCEx_SetWakeUpTimer_IT(&hrtc, 0, RTC_WAKEUPCLOCK_CK_SPRE_16BITS) != HAL_OK)
    {
        Error_Handler();
    }
    /* USER CODE BEGIN RTC_Init 2 */

    /* USER CODE END RTC_Init 2 */

}

void HAL_RTC_MspInit(RTC_HandleTypeDef* rtcHandle)
{

    RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};
    if(rtcHandle->Instance==RTC)
    {
        /* USER CODE BEGIN RTC_MspInit 0 */

        /* USER CODE END RTC_MspInit 0 */

        /** Initializes the peripherals clock
        */
        PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
        PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
        if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
        {
            Error_Handler();
        }

        /* RTC clock enable */
        __HAL_RCC_RTC_ENABLE();
        __HAL_RCC_PWR_CLK_ENABLE();
        HAL_PWR_EnableBkUpAccess();

        /* RTC interrupt Init */
        HAL_NVIC_SetPriority(RTC_WKUP_IRQn, 3, 0);
        HAL_NVIC_EnableIRQ(RTC_WKUP_IRQn);
        /* USER CODE BEGIN RTC_MspInit 1 */

        /* USER CODE END RTC_MspInit 1 */
    }
}

void HAL_RTC_MspDeInit(RTC_HandleTypeDef* rtcHandle)
{

    if(rtcHandle->Instance==RTC)
    {
        /* USER CODE BEGIN RTC_MspDeInit 0 */

        /* USER CODE END RTC_MspDeInit 0 */
        /* Peripheral clock disable */
        __HAL_RCC_RTC_DISABLE();

        /* RTC interrupt Deinit */
        HAL_NVIC_DisableIRQ(RTC_WKUP_IRQn);
        /* USER CODE BEGIN RTC_MspDeInit 1 */

        /* USER CODE END RTC_MspDeInit 1 */
    }
}

void HAL_RTCEx_WakeUpTimerEventCallback(RTC_HandleTypeDef *hrtc)
{
    g_rtc_wakeup_event=1;
}

#if 0
void rtc_init(uint8_t mode)
{
    /* Enable the PWR clock */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

    /* Allow access to RTC */
    PWR_BackupAccessCmd(ENABLE);

    RCC_LSEConfig(RCC_LSE_ON);

    while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET);

    RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);

    RCC_RTCCLKCmd(ENABLE);

    RTC_WaitForSynchro();

    /* Configure the RTC data register and RTC prescaler */
    RTC_InitStructure.RTC_AsynchPrediv = 0x7F;
    RTC_InitStructure.RTC_SynchPrediv = 0xFF;
    RTC_InitStructure.RTC_HourFormat = RTC_HourFormat_24;
    RTC_Init(&RTC_InitStructure);

    if(mode == SET_TIME)
    {
        printf("setting time\r\n");
        /* Set the date: Friday July 06th 2024 */
        RTC_DateStructure.RTC_Year = 0x24;
        RTC_DateStructure.RTC_Month = RTC_Month_July;
        RTC_DateStructure.RTC_Date = 0x06;
        RTC_DateStructure.RTC_WeekDay = RTC_Weekday_Saturday;
        RTC_SetDate(RTC_Format_BCD, &RTC_DateStructure);

        /* Set the time to 13h 42mn 00s pM */
        RTC_TimeStructure.RTC_H12     = RTC_H12_PM;
        RTC_TimeStructure.RTC_Hours   = 0x13;
        RTC_TimeStructure.RTC_Minutes = 0x42;
        RTC_TimeStructure.RTC_Seconds = 0x00;
        RTC_SetTime(RTC_Format_BCD, &RTC_TimeStructure);
    }

    //关闭唤醒功能
    RTC_WakeUpCmd(DISABLE);

    //为唤醒功能选择RTC配置好的时钟源
    RTC_WakeUpClockConfig(RTC_WakeUpClock_CK_SPRE_16bits);

    //设置唤醒计数值为自动重载，写入值默认是0
    RTC_SetWakeUpCounter(1-1);

    //清除RTC唤醒中断标志
    RTC_ClearITPendingBit(RTC_IT_WUT);

    //使能RTC唤醒中断
    RTC_ITConfig(RTC_IT_WUT, ENABLE);

    //使能唤醒功能
    RTC_WakeUpCmd(ENABLE);

    /* Configure EXTI Line22，配置外部中断控制线22 */
    EXTI_InitStructure.EXTI_Line = EXTI_Line22;			//当前使用外部中断控制线22
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;		//中断模式
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;		//上升沿触发中断
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;			//使能外部中断控制线22
    EXTI_Init(&EXTI_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = RTC_WKUP_IRQn;		//允许RTC唤醒中断触发
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x03;	//抢占优先级为0x3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x03;		//响应优先级为0x3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//使能
    NVIC_Init(&NVIC_InitStructure);
}

void RTC_WKUP_IRQHandler(void)
{
    if(RTC_GetITStatus(RTC_IT_WUT) != RESET)
    {
        g_rtc_wakeup_event = 1;
        g_adc_conver_event = 1;
        g_dht11_get_event  = 1;
        // printf("RTC_WKUP_IRQHandler\r\n");
        RTC_ClearITPendingBit(RTC_IT_WUT);
        EXTI_ClearITPendingBit(EXTI_Line22);
    }
}
#endif

uint8_t BCD_to_Dec(uint8_t bcd)
{
    return ((bcd >> 4) * 10 + (bcd & 0x0F));
}
