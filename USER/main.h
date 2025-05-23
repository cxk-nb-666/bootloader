#ifndef MAIN_H
#define MAIN_H

// STM32F407芯片Flash存储器配置
#define STM32_FLASH_BASE 0x08000000UL        // Flash起始地址
#define STM32_sector_Num 12                  // Flash扇区总数
#define STM32_0_3_sector_size (16*1024)      // 0-3扇区大小：16KB
#define STM32_4_sector_size (64*1024)        // 4扇区大小：64KB
#define STM32_5_11_sector_size (128*1024)    // 5-11扇区大小：128KB

// Bootloader和应用程序分区定义
#define STM32_B_sector_num 4                  // Bootloader占用扇区数
#define STM32_A_sector_num STM32_sector_Num-STM32_B_sector_num  // 应用程序占用扇区数
#define STM32_A_Start_Sector STM32_B_sector_num+1               // 应用程序起始扇区
#define STM32_A_Start_Addr (uint32_t)(STM32_FLASH_BASE+(STM32_0_3_sector_size*STM32_B_sector_num))
//#define STM32_A_Start_Addr 0x08010000        // 应用程序起始地址：0x08010000
#define OTA_SET_FLAG 0x11223344              // OTA更新标志值

// 应用程序更新标志位
#define UpDataA_Flag 0x00000001

// OTA信息控制块结构体定义
typedef struct{
    uint32_t OTA_Flag;      // OTA标志，用于指示是否需要更新
    uint32_t FireLen[11];   // 固件长度数组，0号成员固定对应OTA的大小，不同的成员对应不同的固件大小
}OTA_InfoCB;
#define OTA_InfoCB_SIZE sizeof(OTA_InfoCB)   // OTA信息控制块大小


// 应用程序更新控制块结构体定义
typedef struct{
    uint8_t UpDataA_Buf[1024];     // 更新缓冲区，用于临时存储从W25Q128读取的数据
    uint32_t W25Q64_BlockNumber;   // W25Q64块号，指示当前操作的存储块
}UpDataA_CB;


// 外部声明OTA信息变量
extern OTA_InfoCB OTA_Info;
extern UpDataA_CB UpDataA;
extern uint32_t BootStatusFlag;

#endif


