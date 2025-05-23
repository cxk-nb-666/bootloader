#include "boot.h"

// 定义函数指针，用于跳转到应用程序入口点
load_a Load_A;

/**
 * @brief  Bootloader分支判断函数
 * @note   根据OTA标志决定是执行OTA更新还是跳转到应用程序
 * @param  无
 * @retval 无
 */
void BootLoader_Branch(void){
    if(BootLoader_Enter(20)==0){
        U1_printf("不进入BootLoader\r\n");  // 打印进入BootLoader信息
        // 检查OTA标志是否设置
        if(OTA_Info.OTA_Flag == OTA_SET_FLAG){
        U1_printf("OTA事件\r\n");        // 打印OTA事件信息
        BootStatusFlag|=UpDataA_Flag;    // 设置更新标志
        UpDataA.W25Q64_BlockNumber=0;    // 初始化块号为0
        }else{
            U1_printf("跳转APP\r\n");      // 打印跳转APP信息
            LOAD_A(STM32_A_Start_Addr);      // 跳转到应用程序起始地址
        }
    }else{
        U1_printf("进入BootLoader\r\n");  // 打印不进入BootLoader信息
        BootLoader_Info();
    }
    
    
}

uint8_t BootLoader_Enter(uint8_t time){
    U1_printf("在%d秒内输入w,进入BootLoader命令\r\n",time/10);
    while(time--){
        delay_ms(100);
        if(USART1_RxBuffer[0] == 'w'){
            return 1;
        }
    }
    U1_printf("不进入BootLoader命令\r\n");
    return 0;
}

void BootLoader_Info(void){
    U1_printf("BootLoader信息\r\n");
    U1_printf("[1]擦除A区\r\n");
    U1_printf("[2]串口IAP下载A区程序\r\n");
    U1_printf("[3]设置OTA版本号\r\n");
    U1_printf("[4]查询OTA版本号\r\n");
    U1_printf("[5]向外部Flash下载程序\r\n");
    U1_printf("[6]使用外部Flash内程序\r\n");
    U1_printf("[7]重启\r\n");
}



/**
 * @brief  设置主堆栈指针的汇编函数
 * @note   使用汇编指令直接设置MSP寄存器
 * @param  Addr: 新的堆栈指针地址
 * @retval 无
 */
__asm void MSR_SP(uint32_t Addr){
    MSR MSP, r0    // 将r0寄存器的值(Addr)加载到MSP寄存器
    BX r14         // 返回调用函数
}

/**
 * @brief  加载并跳转到应用程序
 * @note   检查应用程序起始地址的有效性，设置堆栈指针并跳转
 * @param  Addr: 应用程序起始地址
 * @retval 无
 */
void LOAD_A(uint32_t Addr){
    // 检查应用程序向量表的堆栈指针是否在有效的SRAM范围内
    if((*(uint32_t *)Addr>=0x20000000)&&(*(uint32_t *)Addr<=(0x20000000+0x20000-1))){
        MSR_SP(*(uint32_t *)Addr);                // 设置主堆栈指针
        Load_A = (load_a)*(uint32_t *)(Addr+4);  // 获取复位向量(应用程序入口点)
        Load_A();                                 // 跳转到应用程序
    }
}

/**
 * @brief  清理Bootloader使用的外设
 * @note   在跳转到应用程序前释放GPIO和USART资源
 * @param  无
 * @retval 无
 */
void BootLoader_Clear(void){
    GPIO_DeInit(GPIOB);    // 复位GPIOB
    GPIO_DeInit(GPIOA);    // 复位GPIOA
    USART_DeInit(USART1);  // 复位USART1
}



