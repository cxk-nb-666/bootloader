#ifndef _W25Q128_H
#define _W25Q128_H

#include "SPI.h"

void W25Q128_Init(void);
void W25Q128_WaitBusy(void);
void W25Q128_Erase64K(uint8_t BlockNumber);
void W25Q128_PageProgram(uint8_t *wbuff ,uint8_t PageNumber);
void W25Q128_ReadData(uint8_t *rbuff,uint32_t Addr,uint32_t Length);
void W25Q128_Enable(void);
void W25Q128_Disable(void);

#endif









