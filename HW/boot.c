#include "boot.h"

// 定义函数指针，用于跳转到应用程序入口点
load_a Load_A;

/**
 * @brief  Bootloader分支判断函数
 * @note   根据OTA标志决定是执行OTA更新还是跳转到应用程序
 * @param  无
 * @retval 无
 */
void BootLoader_Branch(void)
{
    if (BootLoader_Enter(20) == 0)
    {
        U1_printf("不进入BootLoader\r\n"); // 打印进入BootLoader信息
        // 检查OTA标志是否设置
        if (OTA_Info.OTA_Flag == OTA_SET_FLAG)
        {
            U1_printf("OTA事件\r\n");       // 打印OTA事件信息
            BootStatusFlag |= UpDataA_Flag; // 设置更新标志
            UpDataA.W25Q64_BlockNumber = 0;
        }
        else
        {
            U1_printf("跳转APP\r\n");   // 打印跳转APP信息
            LOAD_A(STM32_A_Start_Addr); // 跳转到应用程序起始地址
        }
    }
    else
    {
        U1_printf("进入BootLoader\r\n"); // 打印进入BootLoader信息
        BootLoader_Info();
    }
}

uint8_t BootLoader_Enter(uint8_t time)
{
    U1_printf("在%d秒内输入w,进入BootLoader\r\n", time / 10);
    while (time--)
    {
        delay_ms(100);
        if (USART1_RxBuffer[0] == 'w')
        {
            return 1;
        }
    }
    U1_printf("不进入BootLoader\r\n");
    return 0;
}

void BootLoader_Info(void)
{
    U1_printf("BootLoader信息\r\n");
    U1_printf("[1]擦除A区\r\n");
    U1_printf("[2]串口IAP下载A区程序\r\n");
    U1_printf("[3]设置OTA版本号\r\n");
    U1_printf("[4]查询OTA版本号\r\n");
    U1_printf("[5]向外部Flash下载程序\r\n");
    U1_printf("[6]使用外部Flash内程序\r\n");
    U1_printf("[7]重启\r\n");
}

void BootLoader_Event(uint8_t *data, uint8_t datalen)
{
    if (BootStatusFlag == 0)
    {
        if ((data[0] == '1') && (datalen == 1))
        {
            FLASH_Erase(STM32_A_Start_Sector, STM32_A_sector_num);
            U1_printf("擦除完成\r\n");
        }
        else if ((data[0] == '2') && (datalen == 1))
        {
            U1_printf("通过Xmdoem协议,串口IAP下载A区程序，使用.bin格式\r\n");
            FLASH_Erase(STM32_A_Start_Sector, STM32_A_sector_num);
            BootStatusFlag |= (IAP_Xmodem_Flag | IAP_Xmodem_Data);
            UpDataA.Xmodem_timer = 0;
            UpDataA.Xmodem_RecvNum = 0;
        }
        else if ((data[0] == '3') && (datalen == 1))
        {
            U1_printf("请设置OTA版本号，格式为VERSION-V1.0-2025-05-29-11:38\r\n");
            BootStatusFlag |= IAP_SET_OTA_VRESION;
        }
        else if ((data[0] == '4') && (datalen == 1))
        {
            BootStatusFlag |= IAP_GET_OTA_VRESION;
            AT24C02_Read_OTA_Info();
            U1_printf("当前版本号为:%s\r\n", OTA_Info.OTA_Version);
            BootStatusFlag &= ~IAP_GET_OTA_VRESION;
            BootLoader_Info();
        }
        else if ((data[0] == '5') && (datalen == 1))
        {
            BootStatusFlag |= IAP_DOWNLOAD_TO_OUT_FLASH;
            UpDataA.Xmodem_timer = 0;
            UpDataA.Xmodem_RecvNum = 0;
            U1_printf("请输入要下载到的FLASH块号:0~9\r\n");
        }
        else if ((data[0] == '6') && (datalen == 1))
        {
            BootStatusFlag |= IAP_USE_OTA;
            U1_printf("请输入块号(0~9)，以使用当中的程序\r\n");
        }
        else if ((data[0] == '7') && (datalen == 1))
        {
            U1_printf("即将重启\r\n");
            delay_ms(1000);
            NVIC_SystemReset(); // 复位
        }else if (data[0] == '8' && datalen == 1)
        {
           
           U1_printf("块号1的长度为%d",OTA_Info.FireLen[1]);
        }
    }else
   if (BootStatusFlag & IAP_Xmodem_Data) // 串口IAP下载A区程序
    {
        if (data[0] == 0x01 && datalen == 133)
        {
            BootStatusFlag &= ~IAP_Xmodem_Flag;
            UpDataA.XmodemCRC = Xmodem_CRC16(&data[3], 128);
            if (UpDataA.XmodemCRC == (data[131] * 256 + data[132]))
            {
                UpDataA.Xmodem_RecvNum++;
                if (BootStatusFlag & IAP_DOWNLOAD_TO_OUT_FLASH)
                {
                    OTA_Info.FireLen[UpDataA.W25Q64_BlockNumber] += 128;                               // 收到一个数据包
                    memcpy(&UpDataA.UpDataA_Buf[((UpDataA.Xmodem_RecvNum - 1) % 2) * 128], &data[3], 128); // copy到缓冲区中
                    if (UpDataA.Xmodem_RecvNum % 2 == 0)
                    {
                        W25Q128_PageProgram(UpDataA.UpDataA_Buf, UpDataA.W25Q64_BlockNumber * 256 + (UpDataA.Xmodem_RecvNum / 2 - 1));
                        //memset(UpDataA.UpDataA_Buf, 0, 256); // 清空缓冲区
                    }
                }
                else
                {
                    memcpy(&UpDataA.UpDataA_Buf[((UpDataA.Xmodem_RecvNum - 1) % 8) * 128], &data[3], 128); // copy到缓冲区中
                    if (UpDataA.Xmodem_RecvNum % 8 == 0)                                                   // 收到8个数据包就写入flash
                    {
                        FLASH_Write(STM32_A_Start_Addr + (UpDataA.Xmodem_RecvNum / 8 - 1) * 1024, UpDataA.UpDataA_Buf, 1024);
                    }
                } // 收到一个数据包
                U1_printf("\x06"); // ACK
            }
            else
            {
                U1_printf("\x15"); // NOT ACK
            }
        }
        if (data[0] == 0x04 && datalen == 1)// 接收到EOF
        {                      
            U1_printf("\x06"); // 应答
            if (BootStatusFlag & IAP_DOWNLOAD_TO_OUT_FLASH)
            {
                if (UpDataA.Xmodem_RecvNum % 2 != 0)
                {
                    W25Q128_PageProgram(UpDataA.UpDataA_Buf, UpDataA.W25Q64_BlockNumber * 256 + UpDataA.Xmodem_RecvNum / 2);
                }
                AT24C02_Write_OTA_Info();
                BootStatusFlag &= ~ (IAP_DOWNLOAD_TO_OUT_FLASH | IAP_Xmodem_Data);
                U1_printf("下载完成\r\n");
                BootLoader_Info();
            }
            else if (UpDataA.Xmodem_RecvNum % 8 != 0) // 处理不足1024个字节的数据包
            {
                FLASH_Write(STM32_A_Start_Addr + (UpDataA.Xmodem_RecvNum / 8) * 1024, UpDataA.UpDataA_Buf, (UpDataA.Xmodem_RecvNum % 8) * 128);
                BootStatusFlag &= ~IAP_Xmodem_Data;
                delay_ms(1000);
                NVIC_SystemReset(); // 复位
            }
        }
    }
    else if (BootStatusFlag & IAP_SET_OTA_VRESION) // 设置OTA版本号
    {
        uint8_t temp = 0;
        if (datalen == 28)
        {
            // 检查字母格式
            if (data[0] == 'V' && data[1] == 'E' && data[2] == 'R' && data[3] == 'S' && data[4] == 'I' && data[5] == 'O' && data[6] == 'N' && data[7] == '-')
            {
                // 对比格式，如果7个格式都正确，就设置版本号
                if (sscanf((char *)data, "VERSION-V%d.%d-20%d-%d-%d-%d:%d", &temp, &temp, &temp, &temp, &temp, &temp, &temp) == 7)
                {
                    memcpy(&OTA_Info.OTA_Version, &data[0], 28);
                    AT24C02_Write_OTA_Info();
                    U1_printf("设置成功\r\n");
                    BootStatusFlag &= ~IAP_SET_OTA_VRESION;
                    BootLoader_Info();
                }
                else
                {
                    U1_printf("格式错误\r\n");
                }
            }
            else
            {
                U1_printf("字母错误\r\n");
            }
        }
        else
        {
            U1_printf("长度错误\r\n");
        }
    }
    else if (BootStatusFlag & IAP_DOWNLOAD_TO_OUT_FLASH) // 向外部Flash下载程序
    {
        // 判断输入的块号是否合法
        if ((data[0] >= 0x31) && (data[0] <= 0x39) && (datalen == 1))
        {
            UpDataA.W25Q64_BlockNumber = data[0] - 0x30;
            OTA_Info.FireLen[UpDataA.W25Q64_BlockNumber] = 0;
            W25Q128_Erase64K(UpDataA.W25Q64_BlockNumber);
            U1_printf("将要下载到的FLASH块号:%d\r\n", UpDataA.W25Q64_BlockNumber);
            BootStatusFlag |= (IAP_Xmodem_Data | IAP_Xmodem_Flag);
            UpDataA.Xmodem_timer = 0;
            UpDataA.Xmodem_RecvNum = 0;
        }
        else
        {
            U1_printf("块号错误\r\n");
        }
    }
    else if (BootStatusFlag & IAP_USE_OTA)
    {
        uint32_t i;
        
        if ((data[0] >= '1') && (data[0] <= '9') && (datalen == 1))
        {
            FLASH_Erase(STM32_A_Start_Sector, STM32_A_sector_num);
            U1_printf("即将使用块号为%d的程序\r\n", data[0] - '0');
						AT24C02_Read_OTA_Info();
            UpDataA.W25Q64_BlockNumber = data[0] - 0x30;
            for (i = 0; i < OTA_Info.FireLen[UpDataA.W25Q64_BlockNumber] / 1024; i++)
            {
                W25Q128_ReadData(UpDataA.W25Q64_BlockNumber * 64 * 1024 + i * 1024, UpDataA.UpDataA_Buf, 1024);
                FLASH_Write(STM32_A_Start_Addr + i * 1024, UpDataA.UpDataA_Buf, 1024);
								memset(UpDataA.UpDataA_Buf,0,1024);
            }
            if (OTA_Info.FireLen[UpDataA.W25Q64_BlockNumber] % 1024 != 0)
            {
                W25Q128_ReadData(UpDataA.W25Q64_BlockNumber * 64 * 1024 + i * 1024, UpDataA.UpDataA_Buf, OTA_Info.FireLen[UpDataA.W25Q64_BlockNumber] % 1024);
                FLASH_Write(STM32_A_Start_Addr + i * 1024, UpDataA.UpDataA_Buf, OTA_Info.FireLen[UpDataA.W25Q64_BlockNumber] % 1024);
            }
            U1_printf("写入完成\r\n");
            BootStatusFlag &= ~IAP_USE_OTA;
            BootLoader_Info();
        }
    }
}
/**
 * @brief  设置主堆栈指针的汇编函数
 * @note   使用汇编指令直接设置MSP寄存器
 * @param  Addr: 新的堆栈指针地址
 * @retval 无
 */
__asm void MSR_SP(uint32_t Addr)
{

    MSR MSP, r0         // 将r0寄存器的值(Addr)加载到MSP寄存器
     BX r14 // 返回调用函数
}

/**
 * @brief  加载并跳转到应用程序
 * @note   检查应用程序起始地址的有效性，设置堆栈指针并跳转
 * @param  Addr: 应用程序起始地址
 * @retval 无
 */
void LOAD_A(uint32_t Addr)
{
    // 检查应用程序向量表的堆栈指针是否在有效的SRAM范围内
    if ((*(uint32_t *)Addr >= 0x20000000) && (*(uint32_t *)Addr <= (0x20000000 + 0x20000 - 1)))
    {
        MSR_SP(*(uint32_t *)Addr);                  // 设置主堆栈指针
        Load_A = (load_a) * (uint32_t *)(Addr + 4); // 获取复位向量(应用程序入口点)
        Load_A();                                   // 跳转到应用程序
    }
    else
    {
        U1_printf("跳转A分区失败\r\n");
    }
}

/**
 * @brief  清理Bootloader使用的外设
 * @note   在跳转到应用程序前释放GPIO和USART资源
 * @param  无
 * @retval 无
 */
void BootLoader_Clear(void)
{
    GPIO_DeInit(GPIOB);   // 复位GPIOB
    GPIO_DeInit(GPIOA);   // 复位GPIOA
    USART_DeInit(USART1); // 复位USART1
}

uint32_t Xmodem_CRC16(uint8_t *data, uint8_t len)
{
    uint16_t CRC_Init = 0x0000; // CRC初始值
    uint16_t CRC_poly = 0x1021; // CRC校验多项式
    while (len--)
    {                                       // 循环处理每个字节的CRC计算
        CRC_Init = CRC_Init ^ (*data << 8); // 将字节与CRC初始值异或
        for (uint8_t i = 0; i < 8; i++)
        {
            if (CRC_Init & 0x8000)
            {                                          // 检查最高位是否为1,如果是则进行CRC计算
                CRC_Init = (CRC_Init << 1) ^ CRC_poly; // 如果最高位为1，则左移一位并与多项式异或
            }
            else
            {
                CRC_Init = CRC_Init << 1; // 如果最高位为0，则左移一位
            }
        }
        data++; // 下一个字节
    }
    return CRC_Init; // 返回计算得到的CRC值
}
