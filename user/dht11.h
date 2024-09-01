#ifndef __DHT11_H__
#define __DHT11_H__

#include "main.h"

#define DHT11_Pin  GPIO_PIN_9
#define DHT11_port GPIOG

#define DHT11_OUT PGout(9)
#define DHT11_IN  PGin(9)

// extern void dht11_init(void);
extern int8_t dht11_get_data(uint8_t *buf);
extern int8_t get_temp(uint8_t *buf);
extern int8_t get_humi(uint8_t *buf);

#endif
