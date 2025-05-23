#include "fmc.h"

/**
 * @brief  擦除Flash扇区
 * @note   擦除指定数量的连续扇区
 * @param  start: 开始擦除的扇区号
 * @param  count: 要擦除的扇区数量
 * @retval 无
 */
void FLASH_Erase(uint8_t start, uint8_t count){
    FLASH_Unlock();  // 解锁Flash
    for(uint8_t i=0; i<count;i++){
        FLASH_EraseSector(start+i, VoltageRange_1);  // 擦除扇区
    }

    FLASH_Lock();  // 锁定Flash
}

/**
 * @brief  写入数据到Flash
 * @note   将数据写入指定地址的Flash
 * @param  Addr: 写入的起始地址
 * @param  data: 要写入的数据指针
 * @param  count: 要写入的数据量(字)
 * @retval 无
 */
void FLASH_Write(uint8_t Addr, uint32_t *data, uint32_t count){
    FLASH_Unlock();  // 解锁Flash
    for(uint32_t i=0;i<count;i++){
        FLASH_ProgramWord(Addr, data[i]);  // 按字(32位)写入数据
    }
    FLASH_Lock();  // 锁定Flash
}









