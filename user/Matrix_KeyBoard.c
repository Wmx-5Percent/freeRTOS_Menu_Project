#include "main.h"

#define NUM_ROWS 4
#define NUM_COLS 4

volatile uint32_t sysTimeCnt = 0; // 全局变量，用于记录系统运行时间
volatile uint32_t milliseconds = 0;
volatile uint32_t seconds = 0;
volatile uint32_t minutes = 0;
volatile uint32_t hours = 0;
volatile uint32_t days = 0;

extern EventGroupHandle_t g_keyboard_event_group;

extern TaskHandle_t app_task_keyboard_handle;

extern void dgb_printf_safe(const char *format, ...);

GPIO_TypeDef* COL_PORT_GROUP[4] = {GPIOC, GPIOG, GPIOA, GPIOB};
GPIO_TypeDef* ROW_PORT_GROUP[4] = {GPIOC, GPIOB, GPIOE, GPIOA};

uint16_t COL_PINS[4] = {GPIO_PIN_7, GPIO_PIN_15, GPIO_PIN_4, GPIO_PIN_7};
uint16_t ROW_PINS[4] = {GPIO_PIN_9, GPIO_PIN_6, GPIO_PIN_6, GPIO_PIN_8};

#if 0
int8_t matrix_key_scan(void)
{
	uint8_t t;
	int8_t key_val = -1;
	uint8_t col, row;
	uint16_t col_state = 0, new_col_state = 0;
	
	for(row = 0; row < 4; row++)
	{
		HAL_GPIO_WritePin(ROW_PORT_GROUP[row], ROW_PINS[row], GPIO_PIN_RESET);
		
		col_state = 0;
				
		for(col = 0; col < 4; col++)
			col_state |= HAL_GPIO_ReadPin(COL_PORT_GROUP[col], COL_PINS[col]) << col;

        // dgb_printf_safe("col_state:%x\r\n", col_state);

#if 1
		switch(col_state)
		{			
			case 0xE:	 // 第1列按下
			case 0xD:    // 第2列按下
			case 0xB:    // 第3列按下
			case 0x7:    // 第4列按下
                t = 0;
				while(t < 20)
				{
					t++;
					delay_ms(1);  // 延迟一段时间
				}
                t = 0;
				while(t < 10)
				{
					t++;
					delay_ms(1);  // 延迟一段时间
				}

				while(1) {
					for(col = 0; col < 4; col++)
						new_col_state |= HAL_GPIO_ReadPin(COL_PORT_GROUP[col], COL_PINS[col]) << col;
					if(new_col_state == 0xF)  // 所有列均为高电平，按键已弹起
						break;
					else if(new_col_state != col_state) // 按键状态发生变化
						return -1;  // 不处理按键状态变化，返回-1
				}

                // printf("new_col_state: %x\r\n", new_col_state);

                #if 1
				if (col_state == 0xE) 
					key_val = row * 4 + 1;
				else if (col_state == 0xD)
					key_val = row * 4 + 2;
				else if (col_state == 0xB)
					key_val = row * 4 + 3;
				else if (col_state == 0x7)
                {
                    // printf("[0x7]new_col_state: %x\r\n", new_col_state);
					key_val = row * 4 + 4;
                }
				break;
                #endif
			default:
				break;
		}
#endif
		
		if(key_val != -1)
			break;	

		
		HAL_GPIO_WritePin(ROW_PORT_GROUP[row], ROW_PINS[row], GPIO_PIN_SET);
	}
	
	return key_val;
}
#else
int8_t matrix_key_scan(void)
{
    uint8_t t;
    int8_t key_val = -1;
    uint8_t col, row;
    uint16_t col_state = 0, new_col_state = 0;

    for (row = 0; row < NUM_ROWS; row++)
    {
        HAL_GPIO_WritePin(ROW_PORT_GROUP[row], ROW_PINS[row], GPIO_PIN_RESET);

        col_state = 0;

        for (col = 0; col < NUM_COLS; col++)
            col_state |= HAL_GPIO_ReadPin(COL_PORT_GROUP[col], COL_PINS[col]) << col;

        if (col_state != 0xF)  // 如果有按键按下
        {
            t = 0;
            while (t < 20)
            {
                t++;
                delay_ms(1);  // 按键防抖延迟
            }

            for (col = 0; col < NUM_COLS; col++)
                if ((col_state & (1 << col)) == 0)  // 检测按下的列
                {
                    key_val = row * NUM_COLS + col + 1;  // 计算按键值
                    break;
                }

            while (1)
            {
                new_col_state = 0;
                for (col = 0; col < NUM_COLS; col++)
                    new_col_state |= HAL_GPIO_ReadPin(COL_PORT_GROUP[col], COL_PINS[col]) << col;

                if (new_col_state == 0xF)  // 按键弹起
                    break;
                else if (new_col_state != col_state)  // 按键状态发生变化
                    return -1;
            }
            break;
        }

        HAL_GPIO_WritePin(ROW_PORT_GROUP[row], ROW_PINS[row], GPIO_PIN_SET);
    }

    if (key_val != -1)
    {
        // 使用 xEventGroupSetBits 发送事件组
        xEventGroupSetBits(g_keyboard_event_group, 1 << (key_val - 1));
    }

    return key_val;
}
#endif

char matrix_val_convert(int8_t key_val)
{
	const char val_mapping[] = {
		 '1', '2', '3', 'A', 
		 '4', '5', '6', 'B', 
		 '7', '8', '9', 'C',
		 '*', '0', '#', 'D'};
	
	if(key_val >= 1 && key_val <= 16)
		return val_mapping[key_val - 1];
	else
		return key_val;
}

void print_system_runtime(void)
{
	dgb_printf_safe("%ddays%02dhr%02dmin%02ds%03dms\r\n",
			days, hours, minutes, seconds, milliseconds);	
}

void handle_authenticated_input(char input_val)
{
    uint8_t temp_buf[4];
    uint8_t humi_buf[4];
    int8_t dht11_ret = 0;

    BaseType_t uxHighWaterMark;

    switch (input_val)
    {
    case 'A':
        // 温度查询
        // dht11_ret = get_temp(temp_buf);
        dht11_ret = dht11_get_data(temp_buf);
        if(dht11_ret)
        {
            // uxHighWaterMark = uxTaskGetStackHighWaterMark(app_task_keyboard_handle);
            // dgb_printf_safe("[temp before]water mark:%lu\r\n", uxHighWaterMark);

            // dgb_printf_safe("temp\r\n");
            dgb_printf_safe("temp: %02d.%02d\r\n", temp_buf[2], temp_buf[3]);
            vTaskDelay(300);

            // uxHighWaterMark = uxTaskGetStackHighWaterMark(app_task_keyboard_handle);
            // dgb_printf_safe("[temp after]water mark:%lu\r\n", uxHighWaterMark);
        }
        else
            dgb_printf_safe("get temp failed:%d\r\n", dht11_ret);
        break;
    case 'B':
        // 湿度查询
        // dht11_ret = get_humi(humi_buf);
        dht11_ret = dht11_get_data(temp_buf);
        if(dht11_ret)
        {
            // uxHighWaterMark = uxTaskGetStackHighWaterMark(app_task_keyboard_handle);
            // dgb_printf_safe("[humi before]water mark:%lu\r\n", uxHighWaterMark);

            dgb_printf_safe("humi: %02d.%02d\r\n", temp_buf[0], temp_buf[1]);
            vTaskDelay(300);

            // uxHighWaterMark = uxTaskGetStackHighWaterMark(app_task_keyboard_handle);
            // dgb_printf_safe("[humi after]water mark:%lu\r\n", uxHighWaterMark);
        }
            // dgb_printf_safe("humi: %02d.%02d\r\n", temp_buf[2], temp_buf[3]);
        else
            dgb_printf_safe("get humi failed:%d\r\n", dht11_ret);
        break;
    // case 'C':
    //     // 超声波测距
    //     break;
    case 'D':
    	// 开发板累计运行时间
    	print_system_runtime();
    	break;
    case '*':
    	// 系统复位
    	factory_password_init();
    	// NVIC_SystemReset();
        HAL_NVIC_SystemReset();
    	break;
    case '#':
        // 修改密码
        change_password();
        break;

    default:
        break;
    }
}
