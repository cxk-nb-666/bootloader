#ifndef _IIC_H_
#define _IIC_H_

#include "stm32f4xx.h"
#include "SysTick.h"

#define IIC_SCL_Port GPIOB
#define IIC_SDA_Port GPIOB
#define IIC_SCL_Pin GPIO_Pin_8
#define IIC_SDA_Pin GPIO_Pin_9

#define IIC_SCL_H GPIO_SetBits(IIC_SCL_Port,IIC_SCL_Pin) 
#define IIC_SCL_L GPIO_ResetBits(IIC_SCL_Port,IIC_SCL_Pin) 
#define IIC_SDA_H GPIO_SetBits(IIC_SDA_Port,IIC_SDA_Pin)
#define IIC_SDA_L GPIO_ResetBits(IIC_SDA_Port,IIC_SDA_Pin)
#define IIC_Read_SDA GPIO_ReadInputDataBit(IIC_SDA_Port,IIC_SDA_Pin)

void EEPROM_I2CInit(void);
void IIC_Start(void);
void IIC_Stop(void);
void IIC_SendAck(void);
void IIC_SendNotAck(void);
uint8_t IIC_WaitAck(uint16_t timeout);
void IIC_SendByte(uint8_t byte);
uint8_t IIC_ReadByte(void);
void IIC_SDAInputMode(void);
void IIC_SDAOutMode(void);

#endif


