#include "stm32f4xx.h"
#include "usart.h"

// 接收缓冲区，用于存储接收到的数据
uint8_t USART1_RxBuffer[U1_RX_BUFFER_SIZE]={0};
// 发送缓冲区，用于存储要发送的数据
uint8_t USART1_TxBuffer[2048]={0};
// 指向接收缓冲区的结构体，用于管理接收数据
U1_ptr_RX_Buffptr_Node U1_RXBufptr;

/**
 * @brief  初始化USART1
 * @note   配置USART1的GPIO、通信参数、中断和DMA
 * @param  bandrate: 波特率
 * @retval 无
 */
void Usart1_Init(uint32_t bandrate){
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE); // 开启USART1的时钟
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE); // 开启GPIOA的时钟

    // 配置GPIO引脚
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10; // TX和RX引脚
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;           // 复用功能模式
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;         // 推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;     // 100MHz速度
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;          // 上拉
    GPIO_Init(GPIOA,&GPIO_InitStructure);
    // 复用成USART1
    GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1);  // TX引脚复用为USART1
    GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1); // RX引脚复用为USART1

    // 配置USART1
    USART_InitTypeDef USART_InitStructure;
    USART_InitStructure.USART_BaudRate = bandrate;           // 波特率
    USART_InitStructure.USART_WordLength = USART_WordLength_8b; // 八位数据位
    USART_InitStructure.USART_StopBits = USART_StopBits_1;   // 一位停止位
    USART_InitStructure.USART_Parity = USART_Parity_No;      // 无奇偶校验位
    USART_InitStructure.USART_Mode = USART_Mode_Tx|USART_Mode_Rx; // 收发模式
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // 无硬件流控
    USART_Init(USART1,&USART_InitStructure);
    USART_Cmd(USART1,ENABLE);  // 使能USART1
    // 空闲中断，因为是OTA升级，要在空闲的时候进行升级
    USART_ITConfig(USART1,USART_IT_IDLE,ENABLE);  // 使能空闲中断
    //USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);  // 使能接收中断
    // 优先级配置
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;  // USART1中断通道
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  // 抢占优先级0
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;         // 子优先级0
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;            // 使能中断通道
    NVIC_Init(&NVIC_InitStructure);
    
    USART1_DMA_Init();  // 初始化USART1的DMA
    U1Rx_PrtInit();     // 初始化接收缓冲区指针

    // 清除中断标志，防止初始化后立即进入中断
    USART_ClearITPendingBit(USART1,USART_IT_IDLE);
    USART_ClearFlag(USART1,USART_FLAG_IDLE);
}

/**
 * @brief  串口1 DMA配置初始化
 * @note   配置DMA2的Stream5通道4用于USART1接收
 * @param  无
 * @retval 无
 */
void USART1_DMA_Init(void){
    // 使能DMA2时钟
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2,ENABLE);
 
    DMA_DeInit(DMA2_Stream5);  // 复位DMA配置

    //配置DMA2_Stream5
    DMA_InitTypeDef DMA_InitStructure;
    DMA_StructInit(&DMA_InitStructure);
    DMA_InitStructure.DMA_BufferSize = U1_RX_MAX+1;//单次传输的最大值
    DMA_InitStructure.DMA_Channel = DMA_Channel_4;//通道
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;//外设到内存
    DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
    DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)USART1_RxBuffer;//内存地址
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;//内存地址递增
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;//单次传输的数据大小
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;//循环模式
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&USART1->DR;//外设地址
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;//单次传输的数据大小
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//外设地址不递增
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;//优先级高
    DMA_Init(DMA2_Stream5,&DMA_InitStructure);
    
    // 使能DMA
    DMA_Cmd(DMA2_Stream5,ENABLE);
    USART_DMACmd(USART1,USART_DMAReq_Rx,ENABLE);  // 使能USART1的DMA接收
}

/**
 * @brief  串口1接收缓冲区指针初始化
 * @note   初始化循环缓冲区的管理结构体
 * @param  无
 * @retval 无
 */
void U1Rx_PrtInit(void){
    U1_RXBufptr.U1RXDataIN = &U1_RXBufptr.U1_RxDataPtr[0];
    U1_RXBufptr.U1RXDataOUT = &U1_RXBufptr.U1_RxDataPtr[0];
    U1_RXBufptr.U1RXDataend = &U1_RXBufptr.U1_RxDataPtr[NUM-1];
    U1_RXBufptr.U1RxCount = 0; 
    U1_RXBufptr.U1RXDataIN->start = USART1_RxBuffer;
}  

/**
 * @brief  串口1格式化输出函数
 * @note   类似printf的格式化输出，支持%d、%s、%x等格式
 * @param  format: 格式化字符串
 * @param  ...: 可变参数列表
 * @retval 无
 */
void U1_printf(char *format,...){
    va_list listdata;
    va_start(listdata, format);
    vsprintf((char*)USART1_TxBuffer, format, listdata);
    va_end(listdata);
    for(uint16_t i=0;i<strlen((const char*)USART1_TxBuffer);i++){
        while(USART_GetFlagStatus(USART1,USART_FLAG_TXE) == RESET);
        USART_SendData(USART1,USART1_TxBuffer[i]);
    }
    while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET);  // 等待发送完成

}







