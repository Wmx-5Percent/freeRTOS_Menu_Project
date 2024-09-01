#ifndef __PASSWORD_H_
#define __PASSWORD_H_

#include <stdint.h>
#include <stdbool.h>

#define FLASH_USER_START_ADDR			((uint32_t)0x08040000)
#define FLASH_USER_END_ADDR				((uint32_t)0x0805FFFF)
#define SECTOR_SIZE						((FLASH_USER_END_ADDR - FLASH_USER_START_ADDR))
#define FACTORY_PASSWORD_FLAG_ADDR		((uint32_t)0x08040022)

#define PASSWORD_LENGTH 6
#define PASSWORD_MAX_LENGTH 10
#define FAKE_PASSWORD_LENGTH 10

extern bool authenticated; 
extern uint8_t password_password_index;
extern char input_password[PASSWORD_LENGTH + 1];
extern const char* factory_password_flag;

extern void write_new_password_2_flash(void);
extern void change_password(void);
extern int8_t verify_password(uint8_t *input_password, uint8_t *correct_password, int8_t verify_method);
extern void factory_password_init(void);
extern void modify_flash_data_in_ram(uint8_t *buffer, uint32_t address, const char *new_data);
extern void backup_flash_data(uint8_t *buffer, uint32_t buffer_size);
// extern void hex_to_string(const char *hex, char *string, int length);
extern void hex_to_string(const uint32_t *hex, char *string, int length);

#endif
