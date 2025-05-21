#include "system.h"
#include "SysTick.h"
#include "usart.h"
#include "AT24C02.h"
#include "fmc.h"
#include "main.h"
#include "boot.h"

OTA_InfoCB OTA_Info;
uint8_t buf[10]={0};
int main()
{
	//系统时钟初始化
	SysTick_Init(168);
	Usart1_Init(115200);
	AT24C02_Init();
	AT24C02_Read_OTA_Info();//读取OTA信息
	BootLoader_Branch();//判断是否有OTA事件
	
	
	while(1)
	{
		
	}

}








