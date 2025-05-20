#include "fmc.h"

//start:开始擦除的扇区
void FLASH_Erase(uint8_t start, uint8_t count){
    FLASH_Unlock();
    for(uint8_t i=0; i<count;i++){
        FLASH_EraseSector(start+i, VoltageRange_1);
    }

    FLASH_Lock();
}

//写入flash
//start_sector:开始写入的扇区
//data:写入的内容
//size:写入大小
void FLASH_Write(uint8_t Addr, uint32_t *data, uint32_t count){
    FLASH_Unlock();
    for(uint32_t i=0;i<count;i++){
        FLASH_ProgramWord(Addr, data[i]);
    }
    FLASH_Lock();
}









