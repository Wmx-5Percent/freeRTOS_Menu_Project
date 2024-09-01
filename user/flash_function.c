#include "main.h"


//读取指定地址的半字(16位数据) 
//faddr:读地址 
//返回值:对应数据.
uint32_t STMFLASH_ReadWord(uint32_t faddr)
{
	return *(uint32_t*)faddr; 
}
 
//获取某个地址所在的flash扇区
//addr:flash地址
//返回值:0~11,即addr所在的扇区
uint16_t STMFLASH_GetFlashSector(uint32_t addr)
{
	if(addr<ADDR_FLASH_SECTOR_1)return 		  FLASH_SECTOR_0;
	else if(addr<ADDR_FLASH_SECTOR_2)return  FLASH_SECTOR_1;
	else if(addr<ADDR_FLASH_SECTOR_3)return  FLASH_SECTOR_2;
	else if(addr<ADDR_FLASH_SECTOR_4)return  FLASH_SECTOR_3;
	else if(addr<ADDR_FLASH_SECTOR_5)return  FLASH_SECTOR_4;
	else if(addr<ADDR_FLASH_SECTOR_6)return  FLASH_SECTOR_5;
	else if(addr<ADDR_FLASH_SECTOR_7)return  FLASH_SECTOR_6;
	else if(addr<ADDR_FLASH_SECTOR_8)return  FLASH_SECTOR_7;
	else if(addr<ADDR_FLASH_SECTOR_9)return  FLASH_SECTOR_8;
	else if(addr<ADDR_FLASH_SECTOR_10)return FLASH_SECTOR_9;
	else if(addr<ADDR_FLASH_SECTOR_11)return FLASH_SECTOR_10; 
	return  FLASH_SECTOR_11;	
}

// SPL
#if 0
uint32_t flash_write_data(uint32_t StartSectorAddress, uint32_t *data, uint32_t numOfWords)	
{ 
	FLASH_Status status = FLASH_COMPLETE;	//获取FLASH状态
	uint32_t temp_addr = StartSectorAddress;//写入临时起始地址存放
	
	int soFar = 0;							//记录数据指针的偏移位置
	// int numOfWords = (strlen((const char *)data) / 4) + ((strlen((const char *)data) % 4 ) != 0);//计算写入数据data的字符数
	
	if(temp_addr<STM32_FLASH_BASE||temp_addr%4) return 99;	//非法地址
	
	FLASH_Unlock();									//解锁 
	
	while(soFar<numOfWords)//写数据
	{
		if((status = FLASH_ProgramWord(StartSectorAddress, data[soFar])) == FLASH_COMPLETE)//写入数据
		{ 
			StartSectorAddress += 4;
			soFar++;
		}
	} 

	FLASH_Lock();//上锁
	
	return soFar;
} 

// HAL
#else
uint32_t flash_write_data(uint32_t StartSectorAddress, uint32_t *data, uint32_t numOfWords) 
{
    HAL_StatusTypeDef status = HAL_OK;     // 获取FLASH状态
    uint32_t temp_addr = StartSectorAddress; // 写入临时起始地址存放
    int soFar = 0;                           // 记录数据指针的偏移位置

    // 检查地址是否合法
    if (temp_addr < FLASH_BASE || temp_addr % 4) return 99;

    // 解锁FLASH
    HAL_FLASH_Unlock();

    while (soFar < numOfWords) // 写数据
    {
        status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, StartSectorAddress, data[soFar]);
        if (status == HAL_OK) // 如果写入成功
        {
            StartSectorAddress += 4;
            soFar++;
        }
        else // 如果写入失败，退出循环
        {
            break;
        }
    }

    // 上锁FLASH
    HAL_FLASH_Lock();

    // 返回成功写入的字数
    return soFar;
}

#endif

//从指定地址开始读出指定长度的数据
//StartSectorAddress:起始地址
//Rx_Buf:数据读取存储缓存
//numberOfWords:字(4位)数
void flash_read_data(uint32_t StartSectorAddress, uint32_t *Rx_Buf, uint16_t numberOfWords)   	
{
	numberOfWords -= 1;
	
	while(1)
	{
		*Rx_Buf = *(__IO uint32_t *)StartSectorAddress;
		if(!(numberOfWords--)) break;
		StartSectorAddress += 4;
		Rx_Buf++;
	}
}

// SPL
#if 0
void flash_erase_sector(uint32_t SectorAddress)
{
	FLASH_Unlock();	
	
	/* Device voltage range supposed to be [2.7V to 3.6V], the operation will
	be done by word */ 
	if (FLASH_EraseSector(STMFLASH_GetFlashSector(SectorAddress), VoltageRange_3) != FLASH_COMPLETE)
	{
		printf("erase sector error\r\n");
		while(1);
	}	
	FLASH_Lock();	

}
#else
void flash_erase_sector(uint32_t SectorAddress)
{
    HAL_FLASH_Unlock();  // 解锁FLASH

    FLASH_EraseInitTypeDef EraseInitStruct;
    uint32_t SectorError = 0;

    // 配置擦除结构体
    EraseInitStruct.TypeErase = FLASH_TYPEERASE_SECTORS;
    EraseInitStruct.VoltageRange = FLASH_VOLTAGE_RANGE_3;  // 电压范围 [2.7V to 3.6V]
    EraseInitStruct.Sector = STMFLASH_GetFlashSector(SectorAddress);
    EraseInitStruct.NbSectors = 1;  // 只擦除一个扇区

    // 执行擦除操作，并检查擦除是否成功
    if (HAL_FLASHEx_Erase(&EraseInitStruct, &SectorError) != HAL_OK)
    {
        printf("erase sector error\r\n");
        while(1);  // 进入死循环，表示擦除失败
    }

    HAL_FLASH_Lock();  // 锁定FLASH
}

#endif
