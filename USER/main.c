#include "system.h"
#include "SysTick.h"
#include "usart.h"
#include "AT24C02.h"
#include "fmc.h"
#include "main.h"
#include "boot.h"
#include "W25Q128.h"

// OTA信息控制块，存储OTA标志和固件长度信息
OTA_InfoCB OTA_Info;
// 应用程序更新控制块，存储更新缓冲区和W25Q64块号
UpDataA_CB UpDataA;

// Bootloader状态标志，用于指示当前操作状态
uint32_t BootStatusFlag=0;

int main()
{	
	uint8_t i;
	// 系统初始化
	SysTick_Init(168);		  // 初始化系统滴答定时器，参数168表示系统时钟为168MHz
	Usart1_Init(115200);	  // 初始化USART1，波特率115200
	AT24C02_Init();			  // 初始化AT24C02 EEPROM
	AT24C02_Write_OTA_Info(); // 写入OTA信息到EEPROM
	AT24C02_Read_OTA_Info();  // 从EEPROM读取OTA信息

	BootLoader_Branch(); // 判断是否有OTA事件，如果没有则跳转到APP
	while (1)
	{
		
		delay_ms(10);
		if (U1_RXBufptr.U1RXDataOUT != U1_RXBufptr.U1RXDataIN)
		{
			// 处理接收到的数据
			BootLoader_Event(U1_RXBufptr.U1RXDataOUT->start, U1_RXBufptr.U1RXDataOUT->end - U1_RXBufptr.U1RXDataOUT->start + 1);
			U1_RXBufptr.U1RXDataOUT++;
			if (U1_RXBufptr.U1RXDataOUT == U1_RXBufptr.U1RXDataend)//判断是否到达缓冲区末尾
			{
				U1_RXBufptr.U1RXDataOUT = &U1_RXBufptr.U1_RxDataPtr[0];
			}
		}
		if (BootStatusFlag & IAP_Xmodem_Flag)
		{
			if (UpDataA.Xmodem_timer >= 100)
			{
				U1_printf("C");//发送xmdoem协议C信号
				UpDataA.Xmodem_timer = 0;
			}
			UpDataA.Xmodem_timer++;
		}

		// 检查是否需要更新应用程序
		if ((BootStatusFlag & UpDataA_Flag))
		{
			U1_printf("长度%d", OTA_Info.FireLen[UpDataA.W25Q64_BlockNumber]);
			// 检查固件长度是否为4的倍数（字对齐）
			if (OTA_Info.FireLen[UpDataA.W25Q64_BlockNumber] % 4 == 0)
			{
				// 擦除应用程序区域的Flash
				FLASH_Erase(STM32_A_Start_Sector, STM32_A_sector_num);
				// 按1KB块读取W25Q128中的固件并写入STM32内部Flash
				for (i = 0; i < OTA_Info.FireLen[UpDataA.W25Q64_BlockNumber] / 1024; i++)
				{
					// 从W25Q128读取数据到缓冲区
					W25Q128_ReadData(i * 1024 + UpDataA.W25Q64_BlockNumber * 64 * 1024, UpDataA.UpDataA_Buf, 1024);
					// 将缓冲区数据写入STM32内部Flash
					FLASH_Write(STM32_A_Start_Addr + i * 1024, UpDataA.UpDataA_Buf, 1024);
				}
				// 处理不足1KB的剩余数据
				if (OTA_Info.FireLen[UpDataA.W25Q64_BlockNumber] % 4 != 0)
				{
					W25Q128_ReadData(i * 1024 + UpDataA.W25Q64_BlockNumber * 64 * 1024, UpDataA.UpDataA_Buf, OTA_Info.FireLen[UpDataA.W25Q64_BlockNumber] % 4);
					FLASH_Write(STM32_A_Start_Addr + i * 1024, UpDataA.UpDataA_Buf, OTA_Info.FireLen[UpDataA.W25Q64_BlockNumber] % 4);
				}
				// 如果是第一个块，清除OTA标志
				if (UpDataA.W25Q64_BlockNumber == 0)
				{
					OTA_Info.OTA_Flag = 0;
					AT24C02_Write_OTA_Info();
				}
				// 系统复位，重新启动进入新应用程序
				NVIC_SystemReset();
			}
			else
			{
				// 固件长度错误，清除更新标志
				U1_printf("长度错误");
				BootStatusFlag &= ~UpDataA_Flag;
			}
		}
	}
		
}
