#include "boot.h"

load_a Load_A;

void BootLoader_Branch(void){
    if(OTA_Info.OTA_Flag == OTA_SET_FLAG){
        U1_printf("OTA事件\r\n");
    }else{
        U1_printf("跳转APP\r\n");
        LOAD_A(STM32_A_Start_Addr);
    }
}

__asm void MSR_SP(uint32_t Addr){
    MSR MSP, r0
    BX r14
}

void LOAD_A(uint32_t Addr){
    if((*(uint32_t *)Addr>=0x20000000)&&(*(uint32_t *)Addr<=(0x20000000+0x20000))){
        MSR_SP(*(uint32_t *)Addr);
        Load_A = (load_a)*(uint32_t *)(Addr+4);
        Load_A();
    }
}

void BootLoader_Clear(void){
    GPIO_DeInit(GPIOB);
    GPIO_DeInit(GPIOA);
    USART_DeInit(USART1);
}

















