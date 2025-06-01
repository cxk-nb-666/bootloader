#include "W25Q128.h"

// 定义SPI片选信号控制宏
#define CS_H  GPIO_SetBits(GPIOB,SPI_CS)     // 片选信号高电平(禁用)
#define CS_L  GPIO_ResetBits(GPIOB,SPI_CS)   // 片选信号低电平(使能)

/**
 * @brief  初始化W25Q128 Flash芯片
 * @note   初始化SPI接口并设置片选信号为高电平
 * @param  无
 * @retval 无
 */
void W25Q128_Init(void){
    CS_H;               // 初始状态下禁用片选
    FLASH_SPI_Init();   // 初始化SPI接口
}

/**
 * @brief  等待W25Q128空闲
 * @note   循环检查状态寄存器的忙标志位
 * @param  无
 * @retval 无
 */
void W25Q128_WaitBusy(void){
    uint8_t res;
    do{
        CS_L;                       // 使能片选
        SPI1_ReadWriteByte(0x05);    // 发送读状态寄存器命令
        res = SPI1_ReadWriteByte(0xFF); // 读取状态寄存器
        CS_H;                       // 禁用片选
    }while((res&0x01)==0x01);       // 检查忙标志位
}

/**
 * @brief  使能W25Q128写操作
 * @note   发送写使能命令
 * @param  无
 * @retval 无
 */
void W25Q128_Enable(void){
    W25Q128_WaitBusy();     // 等待Flash空闲
    CS_L;                   // 使能片选
    SPI1_ReadWriteByte(0x06); // 发送写使能命令
    CS_H;                   // 禁用片选
}

/**
 * @brief  禁用W25Q128写操作
 * @note   发送写禁用命令
 * @param  无
 * @retval 无
 */
void W25Q128_Disable(void){
    W25Q128_WaitBusy();     // 等待Flash空闲
    CS_L;                   // 使能片选
    SPI1_ReadWriteByte(0x04); // 发送写禁用命令
    CS_H;                   // 禁用片选
}

/**
 * @brief  擦除W25Q128的64K块
 * @note   擦除指定的64K块
 * @param  BlockNumber: 要擦除的块号
 * @retval 无
 */
void W25Q128_Erase64K(uint8_t BlockNumber){
    uint8_t wdata[4]={0};

    // 准备擦除命令和地址
    wdata[0] = 0xD8;                                // 64K块擦除命令
    wdata[1] = (BlockNumber * 64 * 1024) >> 16;   // 地址高字节
    wdata[2] = (BlockNumber * 64 * 1024) >> 8;    // 地址中字节
    wdata[3] = (BlockNumber * 64 * 1024) >> 0;    // 地址低字节
    W25Q128_WaitBusy();    // 等待Flash空闲
    W25Q128_Enable();      // 使能写操作
    CS_L;                  // 使能片选
    SPI1_WriteBytes(wdata,4); // 发送擦除命令和地址
    CS_H;                  // 禁用片选
    W25Q128_WaitBusy();    // 等待擦除完成
}

/**
 * @brief  W25Q128页编程
 * @note   向指定页写入数据
 * @param  wbuff: 要写入的数据缓冲区
 * @param  PageNumber: 页号
 * @retval 无
 */
void W25Q128_PageProgram(uint8_t *wbuff ,uint32_t PageNumber){
    uint8_t wdata[4]={0};

    // 准备页编程命令和地址
    wdata[0] = 0x02;                        // 页编程命令
    wdata[1] = (PageNumber * 256) >> 16;    // 地址高字节
    wdata[2] = (PageNumber * 256) >> 8;     // 地址中字节
    wdata[3] = (PageNumber * 256) >> 0;     // 地址低字节
    W25Q128_WaitBusy();        // 等待Flash空闲
    W25Q128_Enable();          // 使能写操作
    CS_L;                      // 使能片选
    SPI1_WriteBytes(wdata,4);  // 发送页编程命令和地址
    SPI1_WriteBytes(wbuff,256); // 发送要写入的数据(256字节)
    CS_H;                      // 禁用片选
    W25Q128_WaitBusy();        // 等待编程完成
}

/**
 * @brief  从W25Q128读取数据
 * @note   从指定地址读取指定长度的数据
 * @param  Addr: 读取的起始地址
 * @param  rbuff: 接收数据的缓冲区
 * @param  Length: 要读取的数据长度
 * @retval 无
 */
void W25Q128_ReadData(uint32_t Addr,uint8_t *rbuff,uint32_t Length){
    uint8_t wdata[4] = {0};
    wdata[0] = 0x03;           // 读数据命令
    wdata[1] = Addr >> 16;     // 地址高字节
    wdata[2] = Addr >> 8;      // 地址中字节
    wdata[3] = Addr >> 0;      // 地址低字节
    W25Q128_WaitBusy();        // 等待Flash空闲
    W25Q128_Enable();          // 使能写操作
    CS_L;                      // 使能片选
    SPI1_WriteBytes(wdata,4);  // 发送读命令和地址
    SPI1_ReadBytes(rbuff,Length); // 读取数据
    CS_H;                      // 禁用片选
    W25Q128_WaitBusy();        // 等待操作完成
}








