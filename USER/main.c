#include "system.h"
#include "SysTick.h"
#include "usart.h"
#include "AT24C02.h"
#include "fmc.h"
#include "main.h"

uint32_t wbuff[1024];

int main()
{
	//系统时钟初始化
	SysTick_Init(168);
	Usart1_Init(115200);
	AT24C02_Init();

	for(uint32_t i=0;i<1024;i++){
		wbuff[i]=0x12345678;
	}

	FLASH_Erase(8, 4);
	FLASH_Write(4, wbuff, 1024);
	
	for(uint32_t i=0;i<1024;i++){
		U1_printf("%x\r\n",*(uint32_t *)((0x08060000+128*1024)+i*4));
	}

	while(1)
	{
		
	}

}


