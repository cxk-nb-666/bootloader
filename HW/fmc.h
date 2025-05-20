#ifndef _fmc_H_
#define _fmc_H_

#include "SysTick.h"

void FLASH_Erase(uint8_t start, uint8_t count);
void FLASH_Write(uint8_t Addr, uint32_t *data, uint32_t count);


#endif

