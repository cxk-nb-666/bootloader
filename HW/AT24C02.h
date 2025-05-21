#ifndef _AT24C02_H
#define _AT24C02_H

#include "IIC.h"

#define AT24C02_ADDRESS 0xA0 // 7-bit address for AT24C02
#define AT24C02_Read_Address 0xA1
#define AT24C02_PAGE_SIZE 8
#define AT24C02_MEMORY_SIZE 256 // Total memory size in bytes
#define AT24C02_MAX_WRITE_SIZE 8 // Maximum number of bytes to write in one operation
#define AT24C02_MAX_READ_SIZE 8 // Maximum number of bytes to read in one operation

void AT24C02_Init(void);
uint8_t AT24C02_WriteByte( uint8_t RegAddr,uint8_t data);
uint8_t AT24C02_WritePage( uint8_t RegAddr,uint8_t *data,uint8_t length);
uint8_t AT24C02_ReadByte( uint16_t RegAddr);
uint8_t AT24C02_ReadBytes( uint16_t RegAddr,uint8_t *data,uint16_t length);
void AT24C02_Read_OTA_Info(void);


#endif // _AT24C02_H
