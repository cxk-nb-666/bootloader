#ifndef usart_h
#define usart_h

#include "stdint.h"
#include "stdarg.h"
#include "stdio.h"
#include "string.h"

#define U1_RX_BUFFER_SIZE 2048
#define U1_RX_MAX 256
#define NUM 10



//指向每次接收数据到接收缓冲区的起始地址和结束地址
typedef struct{
    uint8_t *start;
    uint8_t *end;
}U1_RX_Buffptr_Node;

//存储指向接收数据的信息
//U0RXCount代表已使用的缓冲区
//U0_RxDataPtr代表接收了几次
//IN,OUT指针代表接收数据是否被处理
//end指针一直指向末尾
typedef struct{
    uint16_t U1RxCount;
    U1_RX_Buffptr_Node U1_RxDataPtr[NUM];
    U1_RX_Buffptr_Node *U1RXDataIN;
    U1_RX_Buffptr_Node *U1RXDataOUT;
    U1_RX_Buffptr_Node *U1RXDataend;
}U1_ptr_RX_Buffptr_Node;


extern uint8_t USART1_RxBuffer[];
extern U1_ptr_RX_Buffptr_Node U1_RXBufptr;

void Usart1_Init(uint32_t bandrate);
void USART1_DMA_Init(void);
void U1Rx_PrtInit(void);
void U1_printf(char *format,...);

#endif //usart_h


