#include "AT24C02.h"
#include "main.h"
#include "string.h"

void AT24C02_Init(void){
    EEPROM_I2CInit();
}

uint8_t AT24C02_WriteByte(uint8_t RegAddr,uint8_t data){
    IIC_Start();
    IIC_SendByte(AT24C02_ADDRESS);
    if(IIC_WaitAck(100)) return 1;
    IIC_SendByte(RegAddr);
    if(IIC_WaitAck(100)) return 2;
    IIC_SendByte(data);
    if(IIC_WaitAck(100)) return 3;
    IIC_Stop();
    return 0;
}

uint8_t AT24C02_WritePage(uint8_t RegAddr,uint8_t *data){
    IIC_Start();
    IIC_SendByte(AT24C02_ADDRESS);
    if(IIC_WaitAck(100)) return 1;
    IIC_SendByte(RegAddr);
    if(IIC_WaitAck(100)) return 2;
    for(uint8_t i=0;i<8;i++){
        IIC_SendByte(data[i]);
        if(IIC_WaitAck(100)) return 3;
    }
    IIC_Stop();
    return 0;
}

uint8_t AT24C02_ReadByte(uint16_t RegAddr){
    uint8_t data =0;
    IIC_Start();
    IIC_SendByte(AT24C02_ADDRESS);
    IIC_WaitAck(100);
    IIC_SendByte(RegAddr);
    IIC_WaitAck(100);
    IIC_Start();
    IIC_SendByte(AT24C02_Read_Address);
    IIC_WaitAck(100);
    data = IIC_ReadByte();
    IIC_SendNotAck();
    IIC_Stop(); 
    return data;
}

uint8_t AT24C02_ReadBytes(uint16_t RegAddr,uint8_t *data,uint16_t length){
    IIC_Start();
    IIC_SendByte(AT24C02_ADDRESS);
    IIC_WaitAck(100);
    IIC_SendByte(RegAddr);
    IIC_WaitAck(100);
    IIC_Start();
    IIC_SendByte(AT24C02_Read_Address);
    IIC_WaitAck(100);

    for(uint16_t i=0;i<length;i++){
            data[i] = IIC_ReadByte();
            if(i == length - 1) {
                IIC_SendNotAck(); // Last byte, send NACK
            }
            else
                IIC_SendAck();
    }
    IIC_Stop();
   // IIC_SendNotAck();
    return 0;
}

void AT24C02_Read_OTA_Info(void){
    memset(&OTA_Info,0,OTA_InfoCB_SIZE);
    AT24C02_ReadBytes(0,(uint8_t*)&OTA_Info,OTA_InfoCB_SIZE);
}


void AT24C02_Write_OTA_Info(void){
    uint8_t *wptr;
    wptr = (uint8_t *)&OTA_Info;
    for(uint8_t i=0;i<OTA_InfoCB_SIZE/8;i++){
        AT24C02_WritePage(i*8,wptr+i*8);
        delay_ms(10);
    }
}



