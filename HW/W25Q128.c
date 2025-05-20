#include "W25Q128.h"

#define CS_H  GPIO_SetBits(GPIOB,SPI_CS)
#define CS_L  GPIO_ResetBits(GPIOB,SPI_CS)

void W25Q128_Init(void){
    CS_H;
    FLASH_SPI_Init();
}

void W25Q128_WaitBusy(void){
    uint8_t res;
    do{
        CS_L;
        SPI1_ReadWriteByte(0x05);
        res = SPI1_ReadWriteByte(0xFF);
        CS_H;
    }while((res&0x01)==0x01);
}

void W25Q128_Enable(void){
    W25Q128_WaitBusy();
    CS_L;
    SPI1_ReadWriteByte(0x06);
    CS_H;
}

void W25Q128_Disable(void){
    W25Q128_WaitBusy();
    CS_L;
    SPI1_ReadWriteByte(0x04);
    CS_H;
}

void W25Q128_Erase64K(uint8_t BlockNumber){
    uint8_t wdata[4]={0};

    wdata[0] = 0xD8;
    wdata[1] = (BlockNumber * 128 * 1024) >> 16;
    wdata[2] = (BlockNumber * 128 * 1024) >> 8;
    wdata[3] = (BlockNumber * 128 * 1024) >> 0;

    W25Q128_WaitBusy();
    W25Q128_Enable();
    CS_L;
    SPI1_WriteBytes(wdata,4);
    CS_H;
    W25Q128_WaitBusy();
}


void W25Q128_PageProgram(uint8_t *wbuff ,uint8_t PageNumber){
    uint8_t wdata[4]={0};

    wdata[0] = 0x02;
    wdata[1] = (PageNumber * 128 * 1024) >> 16;
    wdata[2] = (PageNumber * 128 * 1024) >> 8;
    wdata[3] = (PageNumber * 128 * 1024) >> 0;

    W25Q128_WaitBusy();
    W25Q128_Enable();
    CS_L;
    SPI1_WriteBytes(wdata,4);
    SPI1_WriteBytes(wbuff,256);
    CS_H;
    W25Q128_WaitBusy();
}

void W25Q128_ReadData(uint8_t *rbuff,uint32_t Addr,uint32_t Length){
    uint8_t wdata[4] = {0};
    wdata[0] = 0x03;
    wdata[1] = Addr >> 16;
    wdata[2] = Addr >> 8;
    wdata[3] = Addr >> 0;
    W25Q128_WaitBusy();
    W25Q128_Enable();
    CS_L;
    SPI1_WriteBytes(wdata,4);
    SPI1_ReadBytes(rbuff,Length);
    CS_H;
    W25Q128_WaitBusy();
}








