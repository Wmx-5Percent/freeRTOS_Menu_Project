#ifndef __MATRIX_KEYBOARD_H__
#define __MATRIX_KEYBOARD_H__

#include <stm32f4xx.h>

#define MATRIX_KEYBOARD_EVENT ()

extern int8_t matrix_key_scan(void);
extern char matrix_val_convert(int8_t key_val);
extern void handle_authenticated_input(char input_val);

#endif
