#include "stm32f4xx.h"
#include "usart.h"

//接收缓冲区
uint8_t USART1_RxBuffer[U1_RX_BUFFER_SIZE]={0};
//发送缓冲区
uint8_t USART1_TxBuffer[2048]={0};
//指向接收缓冲区的结构体
U1_ptr_RX_Buffptr_Node U1_RXBufptr;

void Usart1_Init(uint32_t bandrate){
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);//开启USART1的时钟
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);//开启GPIOA的时钟

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOA,&GPIO_InitStructure);
    //复用成USART1
    GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1);
    GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1);

    //配置USART1
    USART_InitTypeDef USART_InitStructure;
    USART_InitStructure.USART_BaudRate = bandrate;//波特率
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;//八位数据位
    USART_InitStructure.USART_StopBits = USART_StopBits_1;//一位停止位
    USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
    USART_InitStructure.USART_Mode = USART_Mode_Rx|USART_Mode_Tx;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_Init(USART1,&USART_InitStructure);
    USART_Cmd(USART1,ENABLE);
    //空闲中断，因为是OTA升级，要在空闲的时候进行升级
    USART_ITConfig(USART1,USART_IT_IDLE,ENABLE);
    
    //优先级配置
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    USART1_DMA_Init();
    U1Rx_PrtInit(); 

    USART_ClearITPendingBit(USART1,USART_IT_IDLE);
    USART_ClearFlag(USART1,USART_FLAG_IDLE);
}


void USART1_DMA_Init(void){
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2,ENABLE);
 
    DMA_DeInit(DMA2_Stream5);//默认值初始化

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
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;//普通模式
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&USART1->DR;//外设地址
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;//单次传输的数据大小
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//外设地址不递增
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;//优先级高
    DMA_Init(DMA2_Stream5,&DMA_InitStructure);
    DMA_Cmd(DMA2_Stream5,ENABLE);
    USART_DMACmd(USART1,USART_DMAReq_Rx,ENABLE);
}


void U1Rx_PrtInit(void){
    U1_RXBufptr.U1RXDataIN = &U1_RXBufptr.U1_RxDataPtr[0];
    U1_RXBufptr.U1RXDataOUT = &U1_RXBufptr.U1_RxDataPtr[0];
    U1_RXBufptr.U1RXDataend = &U1_RXBufptr.U1_RxDataPtr[NUM-1];
    U1_RXBufptr.U1RxCount = 0; 
    U1_RXBufptr.U1RXDataIN->start = USART1_RxBuffer;
}  

void U1_printf(char *format,...){
    va_list listdata;
    va_start(listdata, format);
    vsprintf((char*)USART1_TxBuffer, format, listdata);
    va_end(listdata);
    for(uint16_t i=0;i<strlen((const char*)USART1_TxBuffer);i++){
        while(USART_GetFlagStatus(USART1,USART_FLAG_TXE) == RESET);
        USART_SendData(USART1,USART1_TxBuffer[i]);
    }
    while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET);
}







