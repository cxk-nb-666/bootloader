#ifndef  _SPI__H_
#define  _SPI__H_

#include "SysTick.h"

#define SPI1_SCK  GPIO_Pin_3
#define SPI1_MISO GPIO_Pin_4
#define SPI1_MOSI GPIO_Pin_5
#define SPI_CS GPIO_Pin_14

void FLASH_SPI_Init(void);
uint8_t SPI1_ReadWriteByte(uint8_t TxData);
void SPI1_WriteBytes(uint8_t *data,uint16_t length);
void SPI1_ReadBytes(uint8_t *data,uint16_t length);

#endif

