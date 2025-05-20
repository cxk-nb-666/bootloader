#include "IIC.h"

void EEPROM_I2CInit(void)
{
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9; // SCL, SDA
    GPIO_Init(GPIOB,&GPIO_InitStructure);
    //GPIO_SetBits(GPIOB,GPIO_Pin_8 | GPIO_Pin_9); // SCL, SDA
}

void IIC_SDAInputMode(void){
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_Pin = IIC_SDA_Pin; // SDA
    GPIO_Init(IIC_SDA_Port,&GPIO_InitStructure);
}

void IIC_SDAOutMode(void){
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_Pin = IIC_SDA_Pin; // SDA
    GPIO_Init(IIC_SDA_Port,&GPIO_InitStructure);
}

void IIC_Start(void){
    IIC_SDA_H;
    IIC_SCL_H;
    delay_us(2);
    IIC_SDA_L;
    delay_us(2);
    IIC_SCL_L;
}

void IIC_Stop(void){
    IIC_SDA_L;
    IIC_SCL_H;
    delay_us(2);
    IIC_SDA_H;
    delay_us(2);
}

void IIC_SendAck(void){
    IIC_SDA_L;
    delay_us(2);
    IIC_SCL_H;
    delay_us(2);
    IIC_SCL_L;
    delay_us(2);
    IIC_SDA_H;
}

void IIC_SendNotAck(void){
    IIC_SDA_H;
    delay_us(2);
    IIC_SCL_H;
    delay_us(2);
    IIC_SCL_L;
    delay_us(2);
}

uint8_t IIC_WaitAck(uint16_t timeout){
    IIC_SDA_H;
    delay_us(2);
    IIC_SCL_H;
    delay_us(2);
    if(IIC_Read_SDA){
        IIC_SCL_L;
        delay_us(2);
        return 1;
    }else{
        IIC_SCL_L;
        delay_us(2);
        return 0;
    }

}
void IIC_SendByte(uint8_t byte){
    IIC_SCL_L;
    for(uint8_t i=0;i<8;i++){
        if(byte&(0x80>>i)){
            IIC_SDA_H;
        }else{
            IIC_SDA_L;
        }
        delay_us(2);
        IIC_SCL_H;
        delay_us(2);
        IIC_SCL_L;
    }
    IIC_SDA_H;
}

uint8_t IIC_ReadByte(void){
    uint8_t byte=0;
    for(uint8_t i=0;i<8;i++){
        IIC_SCL_H;
        delay_us(2);
        if(GPIO_ReadInputDataBit(IIC_SDA_Port,IIC_SDA_Pin)){
            byte = byte | (0x80>>i);
        }
        IIC_SCL_L;
        delay_us(2);
    }
    return byte;
}








