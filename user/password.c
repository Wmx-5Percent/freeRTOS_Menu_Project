#include "main.h"

extern void dgb_printf_safe(const char *format, ...);

bool authenticated = false;

uint8_t password[PASSWORD_LENGTH + 1] = {0};// 正确的密码
uint8_t password_password_index = 0;                 // 计算输入密码的长度
char input_password[PASSWORD_LENGTH + 1];   // 存放用户输入的密码

static uint8_t buffer[SECTOR_SIZE / 3000];    // SECTOR_SIZE太大，131071/3000=43
static uint8_t temp_buffer[SECTOR_SIZE / 3000];
const char* factory_password = "222222";
const char* factory_password_flag = "A";

// 将新数据写入flash
/* 
@new_password: 要写入的新数据 
*/
void write_new_password_2_flash(void)
{
    uint32_t numOfWords;
    uint32_t status;

    numOfWords = ((sizeof(buffer)) + sizeof(uint32_t) - 1) / sizeof(uint32_t);
    // printf("numofwords:%d\r\n", numOfWords);
    flash_erase_sector(FLASH_USER_START_ADDR);

    if((status = flash_write_data(FLASH_USER_START_ADDR, (uint32_t *)buffer, numOfWords)) != numOfWords)
        printf("flash write data failed\r\n");
}

// FLASH扇区数据备份
/*
@buffer: 备份数据存放缓冲区
@buffer_size: 缓冲区大小
*/
void backup_flash_data(uint8_t *buffer, uint32_t buffer_size)
{
    uint32_t i;
	memset(buffer, 0, sizeof(buffer));
    for(i = 0; i < buffer_size; i++)
    {
		*(buffer + i) = *(__IO uint32_t*)(FLASH_USER_START_ADDR + i);
    }
    // printf("backup flash: %s\r\n", buffer);
}

// 修改buffer中的内容用，buffer数据需要写入FLASH
/*
@buffer: 需要修改的缓冲区地址
@address: 需要写入的数据在FLASH扇区中的存放地址
@new_data: 新数据
*/
void modify_flash_data_in_ram(uint8_t *buffer, uint32_t address, const char *new_data)
{
    // uint32_t offset = (address - FLASH_USER_START_ADDR) / sizeof(uint32_t);     //计算数组偏移量，避免覆盖数组中原有的数据
    uint32_t offset = (address - FLASH_USER_START_ADDR);    //计算数组偏移量，避免覆盖数组中原有的数据
    // printf("offset: %d\r\n", offset);

    // 如果为注明新数据内容，则将临时缓冲区整块拷贝至buffer
    if(new_data == NULL)
    {
        memcpy(buffer, temp_buffer, sizeof(temp_buffer));
    }
    // 否则将新数据写入buffer指定位置，通过offset控制写入位置
    else
    {
        // 直接写入新字符串
        uint32_t new_len = strlen(new_data);
        memcpy(&buffer[offset], new_data, new_len);
        // printf("buffer[8]:%c\r\n",buffer[8]);
        return;
    }
}

// 设置出厂密码
void factory_password_init(void)
{
    // 将出厂密码以及出厂设置标志位写入临时缓冲区
    modify_flash_data_in_ram(temp_buffer, FLASH_USER_START_ADDR, factory_password);    // 写入出厂密码
    // delay_ms(500);
    modify_flash_data_in_ram(temp_buffer, FACTORY_PASSWORD_FLAG_ADDR, factory_password_flag);  // 写入出厂密码标志位
    
    // printf("initiate to factory mode\r\n");
    // 备份FLASH中原有数据
    backup_flash_data(buffer, sizeof(buffer));
    // 将temp_buffer的内存块全部拷贝到buffer
    modify_flash_data_in_ram(buffer, FLASH_USER_START_ADDR, NULL);
    // 将buffer中的数据写入FLASH
    write_new_password_2_flash();
}

// 用户输入新密码
void new_password_input()
{
    int8_t key_val;
    char convertion_val;
    int8_t ret_val;
    uint8_t temp_password[PASSWORD_LENGTH + 1];

	while(1)
	{
		key_val = matrix_key_scan();
        // 判断键盘输入
        if(key_val >0 && key_val < 17)
        {
            // 转换键盘输入的数据
            convertion_val = matrix_val_convert(key_val);
            printf("%c", convertion_val);

            // 判断输入的是否为数字
            if(convertion_val >= 48 || convertion_val <= 57)
            {
                // 将转换后的密码放进密码存放缓存
                temp_password[password_password_index++] = convertion_val;
            }

            // 判断输入密码长度
            if(password_password_index == PASSWORD_LENGTH)
            {
                input_password[password_password_index] = '\0';

                password_password_index = 0;

                // 确认新密码
                printf("\r\nplease confirm your input:\r\n");
            
                while (1)
                {
                    key_val = matrix_key_scan();

                    if(key_val >0 && key_val < 17)
                    {
                        // 转换键盘输入的数据
                        convertion_val = matrix_val_convert(key_val);
                        printf("%c", convertion_val);

                        // 判断输入的是否为数字
                        if(convertion_val >= 48 || convertion_val <= 57)
                        {
                            // 将转换后的密码放进密码存放缓存
                            input_password[password_password_index++] = convertion_val;
                        }

                        // 判断输入密码长度
                        if(password_password_index == PASSWORD_LENGTH)
                        {
                            input_password[password_password_index] = '\0';

                            password_password_index = 0;

                            // 验证密码正确性
                            ret_val = verify_password((uint8_t*)input_password, temp_password, 2);

                            // 若与第一次输入的密码不匹配
                            if(ret_val < 0)
                            {
                                printf("The password must be the same for both entries\r\n");
                                continue;
                            }
                            else
                            {
                                // 将新密码以及出厂设置标志位写入buffer
                                modify_flash_data_in_ram(buffer, FLASH_USER_START_ADDR, (const char *)temp_password);
                                modify_flash_data_in_ram(buffer, FACTORY_PASSWORD_FLAG_ADDR, factory_password_flag);  // 写入出厂密码标志位
                                // 将buffer中的数据写入FLASH
                                write_new_password_2_flash();
                                printf("password has been changed\r\n");
                                return;
                            }
                        }
                    }
                }
            }
        }
    }   
}

#if 1
// 更改用户密码
void change_password(void)
{
    char convertion_val;
    int8_t key_val;
    int8_t ret_val;

    // 提示用户输入原密码
    printf("please input the old password:\r\n");
	while(1)
	{
		key_val = matrix_key_scan();
        // 判断键盘输入
        if(key_val >0 && key_val < 17)
        {
            // 转换键盘输入的数据
            convertion_val = matrix_val_convert(key_val);
            printf("%c", convertion_val);

            // 将转换后的密码放进密码存放缓存
            input_password[password_password_index++] = convertion_val;

            // 判断输入密码长度
            if(password_password_index == PASSWORD_LENGTH)
            {
                input_password[password_password_index] = '\0';

                password_password_index = 0;

                // 验证密码正确性
                ret_val = verify_password((uint8_t*)input_password, password, 1);

                // 若输入正确则让用户输入新密码
                if(ret_val > 0)
                {
                    // 确认新密码
                    printf("please input new password:\r\n");
                    new_password_input();
                    break;
                }
                else
                {
                    printf("please input correct old password\r\n");
                    continue;
                }
            }
        }
	}
}
#endif

// 将16进制数转换成字符串
/*
@hex: 16进制数据
@string: 字符串数据存放地址指针
@length: 需要转换的16进制数据长度
*/
#if 0
/* 由于在调用hex_to_string时，第一个参数可能是由uint32_t类型强制转换为const char *类型
   在不考虑跨平台的情况下，可以简单的使用这种方式
   若涉及到跨平台，字节序也许会不一样，所以用第二种方式使用位操作更加保险 */
void hex_to_string(const char *hex, char *string, int length)
{
    int8_t i;

    for (i = 0; i < length; i++)
    {
        string[i] = (char)hex[i];
    }
    string[length] = '\0'; 
}
#else
void hex_to_string(const uint32_t *hex, char *string, int length)
{
    int8_t i, j;
    int8_t index = 0;
    char byte;

    for (i = 0; i < length; i++)
    {
        for(j = 0; j < 4; j++)
        {
            byte = (char)(hex[i] >> (8 * (3 - j)) & 0xFF);
            if(byte != '\0')
            {
                string[index++] = byte;
            }
        }
    }
    string[length] = '\0'; 
}

#endif

// 验证用户输入密码
/* 
@input_password: 用户输入的密码
@correct_password: flash储存的密码
@verify_method: 验证方法
method 1: 与flash储存的密码相比较，用于用户登录系统
method 2: 与用户第一次输入的密码相比较，用于密码修改
    */
int8_t verify_password(uint8_t *input_password, uint8_t *correct_password, int8_t verify_method)
{
    uint32_t temp_password[PASSWORD_LENGTH + 1] = {0};    // 存放从FLASH中读取的密码，使用这种方法，在用户输入一次密码后，程序将跑到hard fault
    
    // 若等于1，则是与FLASH存储的密码比较, 用于用户登录系统密码验证
    if(verify_method == 1)
    {
        flash_read_data(FLASH_USER_START_ADDR, temp_password, PASSWORD_LENGTH + 1);
        // hex_to_string((const char *)temp_password, (char *)correct_password, sizeof(temp_password) / sizeof(temp_password[0]));
        dgb_printf_safe("\r\nsaved password: %s\r\n", correct_password);
        // printf("verifying password: %s, %d\r\n", input_password, strlen(input_password) + 1);
    }

    // 否则是与用户第一次输入的密码比较
    if(strlen((const char *)input_password) == PASSWORD_LENGTH)
    {
        if(strncmp((const char *)input_password, (const char *)correct_password, PASSWORD_LENGTH) == 0)
        {
            authenticated = true;
            dgb_printf_safe("correct password!\r\n");
            beep_short();
            return 1;
        }
        else
        {
            authenticated = false;
            dgb_printf_safe("incorrect password!\r\n");
            beep_long();
            return -1;
        }
    }
    
    // else if(strlen(input_password) == FAKE_PASSWORD_LENGTH)
    // {
    //     printf("fake password\r\n");
    // }
    return 1;
}
