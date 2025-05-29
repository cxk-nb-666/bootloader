#ifndef BOOT_H
#define BOOT_H

// 包含必要的头文件
#include "usart.h"      // 串口通信相关
#include "AT24C02.h"    // EEPROM存储器相关
#include "fmc.h"        // Flash存储器控制相关
#include "main.h"       // 主要配置和定义
#include "stdio.h"      // 标准输入输出库
#include "W25Q128.h"    

// 定义函数指针类型，用于跳转到应用程序
typedef void (*load_a)(void);

// Bootloader主要功能函数声明
void BootLoader_Branch(void);    // Bootloader分支判断函数
__asm void MSR_SP(uint32_t Addr); // 设置主堆栈指针的汇编函数
void LOAD_A(uint32_t Addr);      // 加载并跳转到应用程序
void BootLoader_Clear(void);     // 清理Bootloader使用的外设
uint8_t BootLoader_Enter(uint8_t time);
void BootLoader_Info(void);
void BootLoader_Event(uint8_t *data,uint8_t datalen);
uint32_t Xmodem_CRC16(uint8_t *data,uint8_t len);

#endif



