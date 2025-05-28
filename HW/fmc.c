#include "fmc.h"
#include "usart.h"
/**
 * @brief  擦除Flash扇区
 * @note   擦除指定数量的连续扇区
 * @param  start: 开始擦除的扇区号
 * @param  count: 要擦除的扇区数量
 * @retval 无
 */
void FLASH_Erase(uint8_t start, uint8_t count){
    switch (start)
    {
    case 0 :
        start = FLASH_Sector_0;  // 擦除扇区0
        break;
    case 1 :
        start = FLASH_Sector_1;  // 擦除扇区1
        break;
    case 2 :
        start = FLASH_Sector_2;  // 擦除扇区2
        break;
    case 3 :
        start = FLASH_Sector_3;  // 擦除扇区3
        break;
    case 4 :
        start = FLASH_Sector_4;  // 擦除扇区4
        break;
    case 5 :
        start = FLASH_Sector_5;  // 擦除扇区5
        break;
    case 6 :
        start = FLASH_Sector_6;  // 擦除扇区6
        break;
    case 7 :
        start = FLASH_Sector_7;  // 擦除扇区7
        break;
    case 8 :
        start = FLASH_Sector_8;  // 擦除扇区8
        break;
    case 9 :
        start = FLASH_Sector_9;  // 擦除扇区9
        break;
    case 10 :
        start = FLASH_Sector_10;  // 擦除扇区10
        break;
    case 11 :
        start = FLASH_Sector_11;  // 擦除扇区11
        break;
    default:
        break;
    }
    FLASH_Unlock();  // 解锁Flash
    for(uint8_t i=0; i<count;i++){
       FLASH_EraseSector(start, VoltageRange_3);  // 擦除扇区
       start+=8;
    }
    FLASH_Lock();  // 锁定Flash
}




/**
 * @brief  写入数据到Flash
 * @note   将数据写入指定地址的Flash
 * @param  Addr: 写入的起始地址
 * @param  data: 要写入的数据指针
 * @param  count: 要写入的数据量
 * @retval 无
 */
void FLASH_Write(uint32_t Addr, uint8_t *data, uint32_t count){
    FLASH_Unlock();  // 解锁Flash
    /*
    for(uint32_t i=0; i<count; i++){
        FLASH_ProgramWord(Addr + i*4, data[i]);  
    }
    */
   /*
    while(count){
        FLASH_ProgramWord(Addr, *data);
        Addr+=4;
        data++;
        count-=4;
    }
    */
    while(count){
        FLASH_ProgramByte(Addr, *data);
        Addr+=1;
        data++;
        count--;
    }
    FLASH_Lock();  // 锁定Flash
}




