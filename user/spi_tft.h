#ifndef __SPI_TFT_H_
#define __SPI_TFT_H_

#include "main.h"
#include "spi_tft_font.h"

#define SPI_DC PCout(8)
#define SPI_RST PCout(6)

#define     RED          0XF800	  //红色
#define     GREEN        0X07E0	  //绿色
#define     BLUE         0X001F	  //蓝色
#define     WHITE        0XFFFF	  //白色
#define     BLACK        0x0000   //黑色

#define TFT_COLUMN_NUMBER 240
#define TFT_LINE_NUMBER 240
#define TFT_COLUMN_OFFSET 0

extern void spi_tft_init(void);
extern void TFT_SEND_CMD(uint8_t o_command);
extern void TFT_SEND_DATA(uint8_t o_data);
extern void TFT_SEND_DATA(uint8_t o_data);
extern void TFT_full(uint16_t color);
extern void SPI_SendByte(unsigned  char byte);// 使用硬件SPI
extern void TFT_init(void);
extern void TFT_clear(void);
extern void Picture_Display(const unsigned char *ptr_pic);
extern void display_char16_16(unsigned int x,unsigned int y,unsigned long color,unsigned char word_serial_number);
extern void display_char(uint16_t x,uint16_t y,unsigned long color, uint8_t num, uint8_t size);
extern void show_char(uint16_t x, uint16_t y, uint16_t color, uint8_t *str,uint8_t size);
extern void dma_tft_start(uint8_t *txBuffer, uint8_t *rxBuffer, uint16_t size);

extern uint8_t g_tft_send_buf[128];
extern uint8_t g_tft_recv_buf[128];

#endif
