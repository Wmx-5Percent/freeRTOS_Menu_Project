#include "main.h"
#include <stm32f4xx_hal_dma.h>

extern SPI_HandleTypeDef hspi1;
extern DMA_HandleTypeDef hdma_spi1_tx;

uint8_t g_tft_send_buf[128] = {0};
uint8_t g_tft_recv_buf[128] = {0};
uint8_t g_tft_font_buf[480] = {0};
uint16_t g_buf_index = 0;

void dma_tft_start(uint8_t *txBuffer, uint8_t *rxBuffer, uint16_t size)
{
    uint32_t start = HAL_GetTick();
    uint32_t timeout = 1000;

    SPI_DC = 1; // 设置 SPI 数据指令引脚

    // 确保 SPI 外设已初始化并准备好
    if (HAL_SPI_GetState(&hspi1) != HAL_SPI_STATE_READY)
    {
        // SPI 外设未准备好，返回或处理错误
        Error_Handler();
        return;
    }

    // 确保 DMA 外设已初始化并准备好
    if (HAL_DMA_GetState(&hdma_spi1_tx) != HAL_DMA_STATE_READY)
    {
        // DMA 发送外设未准备好，返回或处理错误
        Error_Handler();
        return;
    }

    if (rxBuffer != NULL)
    {
        // 确保 DMA 接收外设已准备好
        if (HAL_DMA_GetState(&hdma_spi1_tx) != HAL_DMA_STATE_READY)
        {
            // DMA 接收外设未准备好，返回或处理错误
            Error_Handler();
            return;
        }

        // 启动 SPI 的 DMA 全双工传输
        if (HAL_SPI_TransmitReceive_DMA(&hspi1, txBuffer, rxBuffer, size) != HAL_OK)
        {
            // 处理传输错误
            Error_Handler();
            return;
        }
    }
    else
    {
        // 仅启动 SPI 的 DMA 发送传输
        if (HAL_SPI_Transmit_DMA(&hspi1, txBuffer, size) != HAL_OK)
        {
            // 处理传输错误
            Error_Handler();
            return;
        }
    }

    // 等待传输完成
    while (HAL_SPI_GetState(&hspi1) != HAL_SPI_STATE_READY)
    {
        // 您可以在此处添加超时处理代码
        if ((HAL_GetTick() - start) > timeout)
        {
            Error_Handler();
            return;
        }
    }

    // 清除 DMA 传输完成标志
    HAL_DMA_Abort(&hdma_spi1_tx);
    if (rxBuffer != NULL)
    {
        HAL_DMA_Abort(&hdma_spi1_tx);
    }
}

void TFT_SEND_CMD(uint8_t o_command)
{
    SPI_DC = 0;
    // SPI_SCK = 0;
    SPI_SendByte(o_command);
}

//向液晶屏写一个8位数据
void TFT_SEND_DATA(uint8_t o_data)
{
    SPI_DC = 1;
    // SPI_SCK = 0;
    SPI_SendByte(o_data);
}

void TFT_init(void)				////ST7789V2
{
    // SPI_SCK = 1;			//特别注意！！
    SPI_RST = 0;
    delay_us(10);
    SPI_RST = 1;
    delay_us(10);
    TFT_SEND_CMD(0x11); 			//Sleep Out
    delay_ms(120);               //DELAY120ms
    //-----------------------ST7789V Frame rate setting-----------------//
    //************************************************
    TFT_SEND_CMD(0x3A);        //65k mode
    TFT_SEND_DATA(0x05);
    TFT_SEND_CMD(0xC5); 		//VCOM
    TFT_SEND_DATA(0x1A);
    TFT_SEND_CMD(0x36);                 // 屏幕显示方向设置
    TFT_SEND_DATA(0x00);
    //-------------ST7789V Frame rate setting-----------//
    TFT_SEND_CMD(0xb2);		//Porch Setting
    TFT_SEND_DATA(0x05);
    TFT_SEND_DATA(0x05);
    TFT_SEND_DATA(0x00);
    TFT_SEND_DATA(0x33);
    TFT_SEND_DATA(0x33);

    TFT_SEND_CMD(0xb7);			//Gate Control
    TFT_SEND_DATA(0x05);			//12.2v   -10.43v
    //--------------ST7789V Power setting---------------//
    TFT_SEND_CMD(0xBB);//VCOM
    TFT_SEND_DATA(0x3F);

    TFT_SEND_CMD(0xC0); //Power control
    TFT_SEND_DATA(0x2c);

    TFT_SEND_CMD(0xC2);		//VDV and VRH Command Enable
    TFT_SEND_DATA(0x01);

    TFT_SEND_CMD(0xC3);			//VRH Set
    TFT_SEND_DATA(0x0F);		//4.3+( vcom+vcom offset+vdv)

    TFT_SEND_CMD(0xC4);			//VDV Set
    TFT_SEND_DATA(0x20);				//0v

    TFT_SEND_CMD(0xC6);				//Frame Rate Control in Normal Mode
    TFT_SEND_DATA(0X01);			//111Hz

    TFT_SEND_CMD(0xd0);				//Power Control 1
    TFT_SEND_DATA(0xa4);
    TFT_SEND_DATA(0xa1);

    TFT_SEND_CMD(0xE8);				//Power Control 1
    TFT_SEND_DATA(0x03);

    TFT_SEND_CMD(0xE9);				//Equalize time control
    TFT_SEND_DATA(0x09);
    TFT_SEND_DATA(0x09);
    TFT_SEND_DATA(0x08);
    //---------------ST7789V gamma setting-------------//
    TFT_SEND_CMD(0xE0); //Set Gamma
    TFT_SEND_DATA(0xD0);
    TFT_SEND_DATA(0x05);
    TFT_SEND_DATA(0x09);
    TFT_SEND_DATA(0x09);
    TFT_SEND_DATA(0x08);
    TFT_SEND_DATA(0x14);
    TFT_SEND_DATA(0x28);
    TFT_SEND_DATA(0x33);
    TFT_SEND_DATA(0x3F);
    TFT_SEND_DATA(0x07);
    TFT_SEND_DATA(0x13);
    TFT_SEND_DATA(0x14);
    TFT_SEND_DATA(0x28);
    TFT_SEND_DATA(0x30);

    TFT_SEND_CMD(0XE1); //Set Gamma
    TFT_SEND_DATA(0xD0);
    TFT_SEND_DATA(0x05);
    TFT_SEND_DATA(0x09);
    TFT_SEND_DATA(0x09);
    TFT_SEND_DATA(0x08);
    TFT_SEND_DATA(0x03);
    TFT_SEND_DATA(0x24);
    TFT_SEND_DATA(0x32);
    TFT_SEND_DATA(0x32);
    TFT_SEND_DATA(0x3B);
    TFT_SEND_DATA(0x14);
    TFT_SEND_DATA(0x13);
    TFT_SEND_DATA(0x28);
    TFT_SEND_DATA(0x2F);

    TFT_SEND_CMD(0x21); 		//反显

    TFT_SEND_CMD(0x29);         //开启显示
}

void TFT_full(uint16_t color)
{
    uint16_t ROW,column;
    TFT_SEND_CMD(0x2a);     //Column address set
    TFT_SEND_DATA(0x00);    //start column
    TFT_SEND_DATA(0x00);
    TFT_SEND_DATA(0x00);    //end column
    TFT_SEND_DATA(0xF0);

    TFT_SEND_CMD(0x2b);     //Row address set
    TFT_SEND_DATA(0x00);    //start row
    TFT_SEND_DATA(0x00);
    TFT_SEND_DATA(0x00);    //end row
    TFT_SEND_DATA(0xF0);
    TFT_SEND_CMD(0x2C);     //Memory write

    memset(g_tft_send_buf, 0xFF, sizeof(g_tft_send_buf));
    g_buf_index = 0;

    for(ROW = 0; ROW < TFT_LINE_NUMBER; ROW++)             //ROW loop
    {
        for(column = 0; column < TFT_COLUMN_NUMBER; column++) //column loop
        {
            // TFT_SEND_DATA(color >> 8);
            // TFT_SEND_DATA(color);
            g_tft_send_buf[g_buf_index++] = color >> 8;
            g_tft_send_buf[g_buf_index++] = color & 0xFF;
            if(g_buf_index >= sizeof(g_tft_send_buf))
            {
                dma_tft_start(g_tft_send_buf, NULL, sizeof(g_tft_send_buf));
                g_buf_index = 0;
            }
        }
    }
    if(g_buf_index > 0)
    {
        dma_tft_start(g_tft_send_buf, NULL, sizeof(g_tft_send_buf));
        g_buf_index = 0;
    }
}

void TFT_clear(void)
{
    unsigned int ROW,column;
    TFT_SEND_CMD(0x2a);     //Column address set
    TFT_SEND_DATA(0x00);    //start column
    TFT_SEND_DATA(0x00);
    TFT_SEND_DATA(0x00);    //end column
    TFT_SEND_DATA(0xF0);

    TFT_SEND_CMD(0x2b);     //Row address set
    TFT_SEND_DATA(0x00);    //start row
    TFT_SEND_DATA(0x00);
    TFT_SEND_DATA(0x00);    //end row
    TFT_SEND_DATA(0xF0);
    TFT_SEND_CMD(0x2C);     //Memory write

    memset(g_tft_send_buf, 0xFF, sizeof(g_tft_send_buf));
    g_buf_index = 0;

    for(ROW=0; ROW<TFT_LINE_NUMBER; ROW++)           //ROW loop
    {
        for(column=0; column<TFT_COLUMN_NUMBER; column++) //column loop
        {
            // TFT_SEND_DATA(0xFF);
            // TFT_SEND_DATA(0xFF);
            g_tft_send_buf[g_buf_index++] = 0xFF;
            g_tft_send_buf[g_buf_index++] = 0xFF;
            if(g_buf_index >= sizeof(g_tft_send_buf))
            {
                dma_tft_start(g_tft_send_buf, NULL, sizeof(g_tft_send_buf));
                g_buf_index = 0;
            }
        }
    }

    if(g_buf_index > 0)
    {
        dma_tft_start(g_tft_send_buf, NULL, sizeof(g_tft_send_buf));
        g_buf_index = 0;
    }
}

void Picture_Display(const unsigned char *ptr_pic)
{
    unsigned long  number;
    TFT_SEND_CMD(0x2a); 		//Column address set
    TFT_SEND_DATA(0x00); 		//start column
    TFT_SEND_DATA(0x00);
    TFT_SEND_DATA(0x00);		//end column
    TFT_SEND_DATA(0x77);
    TFT_SEND_CMD(0x2b); 		//Row address set
    TFT_SEND_DATA(0x00); 		//start row
    TFT_SEND_DATA(0x00);
    TFT_SEND_DATA(0x00);		//end row
    TFT_SEND_DATA(0x78);
    TFT_SEND_CMD(0x2C);			//Memory write

    for(number=0; number<PIC_NUM; number++)
    {
//            data=*ptr_pic++;
//            data=~data;
        TFT_SEND_DATA(*ptr_pic++);
    }
}

void display_char16_16(unsigned int x,unsigned int y,unsigned long color,unsigned char word_serial_number)
{
    unsigned int column;
    unsigned char tm=0,temp=0,xxx=0;

    TFT_SEND_CMD(0x2a);    //Column address set
    TFT_SEND_DATA(x>>8);    //start column
    TFT_SEND_DATA(x);
    x=x+15;
    TFT_SEND_DATA(x>>8);    //end column
    TFT_SEND_DATA(x);

    TFT_SEND_CMD(0x2b);     //Row address set
    TFT_SEND_DATA(y>>8);    //start row
    TFT_SEND_DATA(y);
    y=y+15;
    TFT_SEND_DATA(y>>8);    //end row
    TFT_SEND_DATA(y);
    TFT_SEND_CMD(0x2C);     //Memory write

    for(column=0; column<32; column++) //column loop
    {
        temp=chines_word[  word_serial_number ][xxx];
        for(tm=0; tm<8; tm++)
        {
            if(temp&0x01)
            {
                TFT_SEND_DATA(color>>8);
                TFT_SEND_DATA(color);
                // g_tft_send_buf[g_buf_index++] = color >> 8;
                // g_tft_send_buf[g_buf_index++] = color;
            }
            else
            {
                TFT_SEND_DATA(0XFF);
                TFT_SEND_DATA(0XFF);
                // g_tft_send_buf[g_buf_index++] = 0xFF;
                // g_tft_send_buf[g_buf_index++] = 0xFF;
            }
            temp>>=1;
        }
        xxx++;
    }
    // dma_tft_start();
    // g_buf_index = 0;
}

void show_char(uint16_t x, uint16_t y, uint16_t color, uint8_t *str,uint8_t size)
{
    while (*str!=0)
    {
        if(x > (TFT_COLUMN_NUMBER - size/2) || y > (TFT_LINE_NUMBER - size)) return;

        display_char(x, y, color, *str, size);
        x += size/2;
        str++;
    }
}

void display_char(uint16_t x,uint16_t y,unsigned long color, uint8_t num, uint8_t size)
{
    unsigned int column;
    unsigned char tm=0,temp=0;

    TFT_SEND_CMD(0x2a);    //Column address set
    TFT_SEND_DATA(x>>8);    //start column
    TFT_SEND_DATA(x);
    x=x+size/2-1;
    TFT_SEND_DATA(x>>8);    //end column
    TFT_SEND_DATA(x);

    TFT_SEND_CMD(0x2b);     //Row address set
    TFT_SEND_DATA(y>>8);    //start row
    TFT_SEND_DATA(y);
    y=y+size-1;
    TFT_SEND_DATA(y>>8);    //end row
    TFT_SEND_DATA(y);
    TFT_SEND_CMD(0x2C);     //Memory write

    num = num-' ';

    for(column=0; column<((size * (size / 2)) / 8); column++) //column loop
    {
        if(size == 16) temp = asc2_1608[num][column];
        if(size == 32) temp = asc2_3216[num][column];
        for(tm=0; tm<8; tm++)
        {
            if(temp&0x01)
            {
                // TFT_SEND_DATA(color>>8);
                // TFT_SEND_DATA(color);
                g_tft_send_buf[g_buf_index++] = color >> 8;
                g_tft_send_buf[g_buf_index++] = color;
            }
            else
            {
                // TFT_SEND_DATA(0XFF);
                // TFT_SEND_DATA(0XFF);
                g_tft_send_buf[g_buf_index++] = 0xFF;
                g_tft_send_buf[g_buf_index++] = 0xFF;
            }
            temp>>=1;
        }
        if (g_buf_index >= sizeof(g_tft_send_buf))
        {
            dma_tft_start(g_tft_send_buf, NULL, sizeof(g_tft_send_buf));
            memset(g_tft_send_buf, 0xFF, sizeof(g_tft_send_buf));
            g_buf_index = 0;
        }
    }
    if (g_buf_index > 0)
    {
        dma_tft_start(g_tft_send_buf, NULL, sizeof(g_tft_send_buf));
        g_buf_index = 0;
    }
}
