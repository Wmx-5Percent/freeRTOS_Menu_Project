#include "dht11.h"

static GPIO_InitTypeDef GPIO_InitStruct = {0};

// void dht11_init(void)
// {
//     //Enable the GPIO AHB clock
//     __HAL_RCC_GPIOG_CLK_ENABLE();
//     //Configure the GPIO pin(s)
//     GPIO_InitStruct.Pin = DHT11_Pin; //set GPIO pins
//     GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;			   //set pin mode, IN/OUT
//     GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;		   //set pin speed, higher speed with higher response but higher consumption
//     GPIO_InitStruct.Pull = GPIO_NOPULL;		   //specify pull-up or pull-down
//     HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);					   //init pin
// }

int8_t dht11_get_data(uint8_t *buf)
{
    uint16_t t;
    int8_t i, j,  byte;
    uint8_t *pbuf;
    uint32_t checksum = 0;

    pbuf = buf;

    GPIO_InitStruct.Pin = DHT11_Pin; //set GPIO pins
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;			   //set pin mode, IN/OUT
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;		   //set pin speed, higher speed with higher response but higher consumption
    GPIO_InitStruct.Pull = GPIO_NOPULL;		   //specify pull-up or pull-down
    HAL_GPIO_Init(DHT11_GPIO_Port, &GPIO_InitStruct);					   //init pin

    PGout(9) = 0;
    delay_ms(25);   // 主机至少拉低18ms
    PGout(9) = 1;
    delay_us(30);   // 主机拉高20-40us

    GPIO_InitStruct.Pin = DHT11_Pin; //set GPIO pins
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;			   //set pin mode, IN/OUT
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;		   //set pin speed, higher speed with higher response but higher consumption
    GPIO_InitStruct.Pull = GPIO_NOPULL;		   //specify pull-up or pull-down
    HAL_GPIO_Init(DHT11_GPIO_Port, &GPIO_InitStruct);					   //init pin

    t = 0;
    while (PGin(9))    // 等待DHT响应（低电平）
    {
        t++;
        delay_us(1);
        if(t >= 100)
            return -1;
    }

    t = 0;
    while (PGin(9) == 0)  // DHT响应信号（低电平）80us
    {
        t++;
        delay_us(1);
        if(t >= 100)
            return -2;
    }

    t = 0;
    while (PGin(9))   // DHT拉高80us
    {
        t++;
        delay_us(1);
        if(t >= 100)
            return -3;
    }

    for(i = 0; i < 5; i++)
    {
        byte = 0;
        for(j = 7; j >= 0; j--)
        {
            t = 0;
            while (PGin(9) == 0)
            {
                t++;
                delay_us(1);
                if(t > 60)
                    return -4;
            }

            delay_us(40);

            if(PGin(9) == 1)
            {
                byte |= (1 << j);

                t = 0;
                while(PGin(9) == 1)
                {
                    t++;
                    delay_us(1);
                    if(t > 60)
                        return -5;
                }
            }
        }
        pbuf[i] = byte;
    }

    checksum = pbuf[0] + pbuf[1] + pbuf[2] + pbuf[3];

    if(checksum != pbuf[4])
        return -6;

    return 1;
}

int8_t get_temp(uint8_t *buf)
{
    // int8_t ret;
    // ret = dht11_get_data(buf);
	// return ret;
    return 1;
}

int8_t get_humi(uint8_t *buf)
{
	return dht11_get_data(buf);
}
