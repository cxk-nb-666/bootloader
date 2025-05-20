#include "SPI.h"

void SPI_PortInit(void){
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);

    GPIO_PinAFConfig(GPIOB,SPI1_SCK,GPIO_AF_SPI1);
    GPIO_PinAFConfig(GPIOB,SPI1_MISO,GPIO_AF_SPI1);
    GPIO_PinAFConfig(GPIOB,SPI1_MOSI,GPIO_AF_SPI1);

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_Pin = SPI1_SCK | SPI1_MISO | SPI1_MOSI;
    GPIO_Init(GPIOB,&GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_Init(GPIOB,&GPIO_InitStructure);
}

void FLASH_SPI_Init(void){
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1,ENABLE);
    SPI_PortInit();

    SPI_InitTypeDef SPI_InitStructure;
    SPI_StructInit(&SPI_InitStructure);
    SPI_InitStructure.SPI_Direction = SPI_Direction_Tx|SPI_Direction_Rx;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
  //SPI_InitStructure.SPI_CRCPolynomial = 
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
    SPI_Init(SPI1,&SPI_InitStructure);
    SPI_Cmd(SPI1,ENABLE);
}

uint8_t SPI1_ReadWriteByte(uint8_t TxData){
    while (SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_TXE)== RESET);
    SPI_I2S_SendData(SPI1,TxData);
    while (SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_RXNE)== RESET);
    return SPI_I2S_ReceiveData(SPI1);
}

void SPI1_WriteBytes(uint8_t *data,uint16_t length){
    for(uint16_t i=0;i<length;i++){
        SPI1_ReadWriteByte(data[i]);
    }
}

void SPI1_ReadBytes(uint8_t *data,uint16_t length){
    for(uint16_t i=0;i<length;i++){
        data[i] = SPI1_ReadWriteByte(0xFF);
    }
}














